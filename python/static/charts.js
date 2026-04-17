let charts = {};

function createOrUpdateChart(key, canvasId, label, labels, data) {
    if (!charts[key]) {
        const ctx = document.getElementById(canvasId).getContext('2d');
        charts[key] = new Chart(ctx, {
            type: 'line',
            data: {
                labels: labels,
                datasets: [{
                    label: label,
                    data: data,
                    borderWidth: 2,
                    fill: false
                }]
            }
        });
    } else {
        charts[key].data.labels = labels;
        charts[key].data.datasets[0].data = data;
        charts[key].update();
    }
}

async function updateCharts() {
    const response = await fetch('/data?ts=' + new Date().getTime());
    const data = await response.json();

    createOrUpdateChart("temp", "tempChart", "Temperature", data.recent.timestamps, data.recent.temp);
    createOrUpdateChart("humi", "humiChart", "Humidity", data.recent.timestamps, data.recent.humi);
    createOrUpdateChart("soil", "soilChart", "Soil", data.recent.timestamps, data.recent.soil);

    createOrUpdateChart("tempDaily", "tempDailyChart", "Temp (Daily Avg)", data.daily.days, data.daily.temp);
    createOrUpdateChart("humiDaily", "humiDailyChart", "Humi (Daily Avg)", data.daily.days, data.daily.humi);
    createOrUpdateChart("soilDaily", "soilDailyChart", "Soil (Daily Avg)", data.daily.days, data.daily.soil);
}

window.addEventListener("resize", () => {
    Object.values(charts).forEach(chart => {
        chart.resize();
    });
});

updateCharts();

setInterval(updateCharts, 5000);