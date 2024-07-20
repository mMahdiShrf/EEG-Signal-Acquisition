import asyncio
import numpy as np
from pymongo import MongoClient
from aiohttp import web
import aiohttp_cors
import mne

# Configuration
IP = '127.0.0.1'
DATA_PORT = 6000
SERVER_PORT = 5000
NCH = 8
NBYTE = NCH * 3 + 3
GAIN = 24
FACTOR = 1 * 0.536441802978515625 / GAIN  # for µV
STORE_SEC = 7  # Duration to store data in seconds
PLOT_SEC = 5   # Duration to plot data in seconds
NSPS = 250
STORE_SAMPLES = STORE_SEC * NSPS
PLOT_SAMPLES = PLOT_SEC * NSPS
NOTCH_FREQS = np.arange(50, 101, 50)  # Frequencies for notch filter

# Suppress MNE logging
mne.set_log_level('WARNING')

# Wave type filter configuration
WAVE_FILTERS = {
    "alpha": {"hp_cutoff": 8, "lp_cutoff": 12},
    "beta": {"hp_cutoff": 12, "lp_cutoff": 30},
    "gamma": {"hp_cutoff": 30, "lp_cutoff": 100},
    "delta": {"hp_cutoff": 0.5, "lp_cutoff": 4},
    "theta": {"hp_cutoff": 4, "lp_cutoff": 8}
}

# Connect to MongoDB
client = MongoClient('mongodb://localhost:27017/')
db = client['eeg_database']
collection = db['eeg_signals']

# Initialize the database with sample data
for i in range(NCH):
    channel_data = {
        "channel": f"CH{i+1}",  # Ensure channel names match client-side requests
        "samples": np.random.randn(STORE_SAMPLES).tolist()
    }
    collection.insert_one(channel_data)

print("EEG data inserted successfully!")

# Function to shift data left and update the last sample
def shift_and_update_channel_data(channel, new_value):
    document = collection.find_one({"channel": channel})
    if document:
        samples = document['samples']
        shifted_samples = samples[1:] + [new_value]
        collection.update_one(
            {"channel": channel},
            {"$set": {"samples": shifted_samples}}
        )

# Client to receive data and update MongoDB
async def client_task():
    client_reader, client_writer = await asyncio.open_connection(host=IP, port=DATA_PORT)
    print("Connection established")
    while True:
        raw_data = await client_reader.readexactly(NBYTE + 1)
        for i in range(NCH):
            new_value = int.from_bytes((raw_data[(4 + i * 3):(7 + i * 3)] + b'\x00'), "big", signed=True) * FACTOR / 256
            channel_name = f"CH{i+1}"  # Ensure channel names match client-side requests
            shift_and_update_channel_data(channel_name, new_value)
        await asyncio.sleep(0)  # Yield control to the event loop

# Function to apply high-pass, low-pass, and notch filters using MNE
def apply_filters(samples, sfreq, hp_cutoff_freq, lp_cutoff_freq, notch_freqs):
    data = np.array(samples).reshape(1, -1)  # MNE expects a 2D array
    info = mne.create_info(ch_names=['eeg'], sfreq=sfreq, ch_types=['eeg'])
    raw = mne.io.RawArray(data, info)
    raw.filter(None, lp_cutoff_freq, filter_length='auto', method='iir', 
               iir_params=dict(order=5, ftype='butter', output='sos'), phase='zero')
    raw.filter(hp_cutoff_freq, None, filter_length='auto', method='iir', 
               iir_params=dict(order=2, ftype='butter', output='sos'), phase='zero')
    raw.notch_filter(notch_freqs, method='fir', phase='zero')
    filtered_samples = raw.get_data()[0]
    return filtered_samples

# Handler to get channel data
async def get_channel_data(request):
    channel = request.query.get('channel')
    if not channel:
        return web.json_response({"error": "Channel not specified"}, status=400)
    
    document = collection.find_one({"channel": channel})
    if not document:
        return web.json_response({"error": "Channel not found"}, status=404)
    
    samples = document['samples']
    filtered_samples = apply_filters(samples, NSPS, 0.6, 80, NOTCH_FREQS)
    # Only take the data from 1 second to 6 seconds
    start_sample = NSPS  # 1 second
    end_sample = 6 * NSPS  # 6 seconds
    filtered_samples = filtered_samples[start_sample:end_sample]
    
    return web.json_response({"channel": channel, "samples": filtered_samples.tolist()}, status=200)

# Handler to get wave type data
async def get_wave_data(request):
    channel = request.query.get('channel')
    wave_type = request.query.get('wave_type')
    
    if not channel or not wave_type:
        return web.json_response({"error": "Channel or wave type not specified"}, status=400)
    
    if wave_type not in WAVE_FILTERS:
        return web.json_response({"error": "Invalid wave type"}, status=400)
    
    document = collection.find_one({"channel": channel})
    if not document:
        return web.json_response({"error": "Channel not found"}, status=404)
    
    samples = document['samples']
    filters = WAVE_FILTERS[wave_type]
    filtered_samples = apply_filters(samples, NSPS, filters["hp_cutoff"], filters["lp_cutoff"], NOTCH_FREQS)
    # Only take the data from 1 second to 6 seconds
    start_sample = NSPS  # 1 second
    end_sample = 6 * NSPS  # 6 seconds
    filtered_samples = filtered_samples[start_sample:end_sample]
    
    return web.json_response({"channel": channel, "samples": filtered_samples.tolist()}, status=200)

# Handler to get PSD data
async def get_psd_data(request):
    channel = request.query.get('channel')
    if not channel:
        return web.json_response({"error": "Channel not specified"}, status=400)
    
    document = collection.find_one({"channel": channel})
    if not document:
        return web.json_response({"error": "Channel not found"}, status=404)
    
    samples = document['samples']
    filtered_samples = apply_filters(samples, NSPS, 0.6, 80, NOTCH_FREQS)
    psd, freqs = calculate_psd(filtered_samples, NSPS)
    
    return web.json_response({"channel": channel, "psd": psd.tolist(), "freqs": freqs.tolist()}, status=200)

# Function to calculate PSD using MNE
def calculate_psd(samples, sfreq, fmin=0, fmax=80, n_fft=512, n_overlap=64, tmin=2.0, tmax=6.0):
    data = np.array(samples).reshape(1, -1)  # MNE expects a 2D array
    info = mne.create_info(ch_names=['eeg'], sfreq=sfreq, ch_types=['eeg'])
    raw = mne.io.RawArray(data, info)
    psd = raw.compute_psd(fmin=fmin, fmax=fmax, n_fft=n_fft, n_overlap=n_overlap)
    psd_data = psd.get_data(return_freqs=True)
    return psd_data

# Create the server application
app = web.Application()
app.router.add_get('/get_channel_data', get_channel_data)
app.router.add_get('/get_wave_data', get_wave_data)  # Add route for wave type data
app.router.add_get('/get_psd_data', get_psd_data)  # Add route for PSD data

# Configure default CORS settings.
cors = aiohttp_cors.setup(app, defaults={
    "*": aiohttp_cors.ResourceOptions(
            allow_credentials=True,
            expose_headers="*",
            allow_headers="*",
        )
})

# Configure CORS on all routes.
for route in list(app.router.routes()):
    cors.add(route)

# Server task to run the web server
async def server_task():
    runner = web.AppRunner(app)
    await runner.setup()
    site = web.TCPSite(runner, 'localhost', SERVER_PORT)
    await site.start()
    print(f"Server started at http://localhost:{SERVER_PORT}")
    while True:
        await asyncio.sleep(3600)

# Main function to run both client and server concurrently
async def main():
    await asyncio.gather(client_task(), server_task())

if __name__ == '__main__':
    asyncio.run(main())
