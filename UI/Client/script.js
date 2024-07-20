document.addEventListener('DOMContentLoaded', (event) => {
    const plotBtn = document.getElementById('plot-btn');
    const stopBtn = document.getElementById('stop-btn');
    const saveBtn = document.getElementById('save-btn');
    const channelSelect = document.getElementById('channel-select');
    const eegCtx = document.getElementById('eeg-plot').getContext('2d');

    const plotWaveBtn = document.getElementById('plot-wave-btn');
    const stopWaveBtn = document.getElementById('stop-wave-btn');
    const saveWaveBtn = document.getElementById('save-wave-btn');
    const waveSelect = document.getElementById('wave-select');
    const waveChannelSelect = document.getElementById('wave-channel-select');  // New dropdown for selecting channels in the second plot
    const waveCtx = document.getElementById('wave-plot').getContext('2d');

    const plotPsdBtn = document.getElementById('plot-psd-btn');
    const stopPsdBtn = document.getElementById('stop-psd-btn');
    const savePsdBtn = document.getElementById('save-psd-btn');
    const psdSelect = document.getElementById('psd-select');  // New dropdown for PSD window
    const psdCtx = document.getElementById('psd-plot').getContext('2d');

    let eegChart = null;
    let waveChart = null;
    let psdChart = null;

    let animationId = null;

    async function fetchData(url) {
        const response = await fetch(url);
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        const data = await response.json();
        return data;
    }

    async function updateEEGData(chart, channel) {
        const url = `http://localhost:5000/get_channel_data?channel=${channel}`;
        const response = await fetchData(url);
        const data = response.samples;
        chart.data.datasets[0].data = data;  // Use the EEG data from the server
        chart.update('none');  // Update the chart without animation
        animationId = requestAnimationFrame(() => updateEEGData(chart, channel));  // Schedule the next frame
    }

    async function updateWaveData(chart, channel, waveType) {
        const url = `http://localhost:5000/get_wave_data?channel=${channel}&wave_type=${waveType}`;
        const response = await fetchData(url);
        const data = response.samples;
        chart.data.datasets[0].data = data;  // Use the wave data from the server
        chart.update('none');  // Update the chart without animation
        animationId = requestAnimationFrame(() => updateWaveData(chart, channel, waveType));  // Schedule the next frame
    }

    async function updatePSDData(chart, channel) {
        const url = `http://localhost:5000/get_psd_data?channel=${channel}`;
        const response = await fetchData(url);
        const psdData = response.psd[0];  // Ensure we're using the first element if psd is nested
        const freqs = response.freqs;
        chart.data.labels = freqs;  // Set the frequencies as labels
        chart.data.datasets[0].data = psdData;  // Use the PSD data from the server
        chart.update();  // Update the chart
    }

    function startPlotting(chart, ctx, channel, xAxisTitle, yAxisTitle, duration, yMin, yMax, sampleRate) {
        const dataLength = duration * sampleRate;  // Simulate data points for the given duration (5 seconds for EEG and Wave, 0 to 100 Hz for PSD)
        const labels = Array.from({ length: dataLength }, (_, i) => i / sampleRate);  // Time in seconds for EEG and Wave, Frequency for PSD

        if (chart) {
            chart.destroy();
        }

        chart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: labels,
                datasets: [{
                    label: channel,
                    data: [],
                    borderColor: 'rgb(75, 192, 192)',
                    fill: false,
                    pointRadius: 0
                }]
            },
            options: {
                responsive: true,
                animation: false,
                scales: {
                    x: {
                        type: 'linear',
                        title: {
                            display: true,
                            text: xAxisTitle
                        },
                        min: 0,
                        max: duration
                    },
                    y: {
                        title: {
                            display: true,
                            text: yAxisTitle
                        },
                        suggestedMin: yMin,
                        suggestedMax: yMax
                    }
                }
            }
        });

        updateEEGData(chart, channel);  // Start fetching and updating EEG data

        return { chart, animationId };
    }

    function startPlottingWave(chart, ctx, channel, waveType, xAxisTitle, yAxisTitle, duration, yMin, yMax, sampleRate) {
        const dataLength = duration * sampleRate;  // Simulate data points for the given duration
        const labels = Array.from({ length: dataLength }, (_, i) => i / sampleRate);  // Time in seconds

        if (chart) {
            chart.destroy();
        }

        chart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: labels,
                datasets: [{
                    label: `${channel} - ${waveType}`,
                    data: [],
                    borderColor: 'rgb(75, 192, 192)',
                    fill: false,
                    pointRadius: 0
                }]
            },
            options: {
                responsive: true,
                animation: false,
                scales: {
                    x: {
                        type: 'linear',
                        title: {
                            display: true,
                            text: xAxisTitle
                        },
                        min: 0,
                        max: duration
                    },
                    y: {
                        title: {
                            display: true,
                            text: yAxisTitle
                        },
                        suggestedMin: yMin,
                        suggestedMax: yMax
                    }
                }
            }
        });

        updateWaveData(chart, channel, waveType);  // Start fetching and updating wave data

        return { chart, animationId };
    }

    function startPlottingPSD(chart, ctx, channel, xAxisTitle, yAxisTitle) {
        if (chart) {
            chart.destroy();
        }

        chart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: [],  // Labels will be updated with frequencies from the server
                datasets: [{
                    label: channel,
                    data: [],
                    borderColor: 'rgb(75, 192, 192)',
                    fill: false,
                    pointRadius: 0
                }]
            },
            options: {
                responsive: true,
                animation: false,
                scales: {
                    x: {
                        type: 'linear',
                        title: {
                            display: true,
                            text: xAxisTitle
                        }
                    },
                    y: {
                        title: {
                            display: true,
                            text: yAxisTitle
                        },
                        suggestedMin: 0,
                        suggestedMax: 100  // Adjust as needed for PSD values
                    }
                }
            }
        });

        updatePSDData(chart, channel);  // Fetch and update PSD data

        return { chart };
    }

    function stopPlotting(animationId) {
        if (animationId) {
            cancelAnimationFrame(animationId);
            animationId = null;
        }
    }

    function saveChartData(chart, chartName) {
        const labels = chart.data.labels;
        const data = chart.data.datasets[0].data;
        let csvContent = "data:text/csv;charset=utf-8,";
        csvContent += "Frequency (Hz),Power (µV²)\n";

        labels.forEach((label, index) => {
            csvContent += label + "," + data[index] + "\n";
        });

        const encodedUri = encodeURI(csvContent);
        const link = document.createElement("a");
        link.setAttribute("href", encodedUri);
        link.setAttribute("download", chartName + ".csv");
        document.body.appendChild(link);
        link.click();
        document.body.removeChild(link);
    }

    plotBtn.addEventListener('click', () => {
        const selectedChannel = channelSelect.value;
        ({ chart: eegChart, animationId } = startPlotting(eegChart, eegCtx, selectedChannel, 'Time (s)', 'Voltage (µV)', 5, -100, 100, 250));
    });

    stopBtn.addEventListener('click', () => {
        stopPlotting(animationId);
    });

    saveBtn.addEventListener('click', () => {
        saveChartData(eegChart, 'EEG_Chart');
    });

    plotWaveBtn.addEventListener('click', () => {
        const selectedWave = waveSelect.value;
        const selectedChannel = waveChannelSelect.value;  // Get the selected channel for the second plot
        ({ chart: waveChart, animationId } = startPlottingWave(waveChart, waveCtx, selectedChannel, selectedWave, 'Time (s)', 'Voltage (µV)', 5, -100, 100, 250));
    });

    stopWaveBtn.addEventListener('click', () => {
        stopPlotting(animationId);
    });

    saveWaveBtn.addEventListener('click', () => {
        saveChartData(waveChart, 'Wave_Chart');
    });

    plotPsdBtn.addEventListener('click', () => {
        const selectedChannel = psdSelect.value;  // Get selected channel for PSD
        ({ chart: psdChart } = startPlottingPSD(psdChart, psdCtx, selectedChannel, 'Frequency (Hz)', 'Power (µV²)'));
    });

    stopPsdBtn.addEventListener('click', () => {
        stopPlotting(animationId);
    });

    savePsdBtn.addEventListener('click', () => {
        saveChartData(psdChart, 'PSD_Chart');
    });
});
