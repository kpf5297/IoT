using System;
using System.Data;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;
using System.Net.Http;
using Newtonsoft.Json.Linq;

namespace SensorDataViewer
{
    public partial class Form1 : Form
    {
        private static readonly HttpClient client = new HttpClient();
        private string flaskServerUrl = "http://192.168.1.152:5000";
        private System.Windows.Forms.Timer autoUpdateTimer;

        public Form1()
        {
            InitializeComponent();

            // Additional initialization
            comboBox1.Items.AddRange(new string[] { "Last 30 Seconds", "Last Minute", "Last Hour", "Last Day", "Last Week" });
            comboBox1.SelectedIndex = 1; // Default selection
            comboBox1.SelectedIndexChanged += comboBox1_SelectedIndexChanged;
            button1.Click += button1_Click;

            autoUpdateTimer = new System.Windows.Forms.Timer();
            autoUpdateTimer.Interval = 500;
            autoUpdateTimer.Tick += autoUpdateTimer_Tick;

            LoadData(flaskServerUrl + "/get_data?range=minute", "Last Minute");  // Default to last minute
        }

        private async void LoadData(string url, string range)
        {
            try
            {
                HttpResponseMessage response = await client.GetAsync(url);
                response.EnsureSuccessStatusCode();
                string responseBody = await response.Content.ReadAsStringAsync();

                JArray dataArray = JArray.Parse(responseBody);
                DataTable table = new DataTable();
                table.Columns.Add("timestamp", typeof(DateTime));
                table.Columns.Add("temperature", typeof(float));
                table.Columns.Add("mic", typeof(int));
                table.Columns.Add("accelX", typeof(float));
                table.Columns.Add("accelY", typeof(float));
                table.Columns.Add("accelZ", typeof(float));
                table.Columns.Add("gyroX", typeof(float));
                table.Columns.Add("gyroY", typeof(float));
                table.Columns.Add("gyroZ", typeof(float));
                table.Columns.Add("pitch", typeof(double));
                table.Columns.Add("roll", typeof(double));

                foreach (var data in dataArray)
                {
                    DataRow row = table.NewRow();
                    row["timestamp"] = data["timestamp"].ToObject<DateTime>();
                    row["temperature"] = data["temperature"].ToObject<float>();
                    row["mic"] = data["mic"].ToObject<int>();
                    row["accelX"] = data["accelX"].ToObject<float>();
                    row["accelY"] = data["accelY"].ToObject<float>();
                    row["accelZ"] = data["accelZ"].ToObject<float>();
                    row["gyroX"] = data["gyroX"].ToObject<float>();
                    row["gyroY"] = data["gyroY"].ToObject<float>();
                    row["gyroZ"] = data["gyroZ"].ToObject<float>();

                    // Calculate pitch and roll
                    double accelX = data["accelX"].ToObject<double>();
                    double accelY = data["accelY"].ToObject<double>();
                    double accelZ = data["accelZ"].ToObject<double>();

                    double pitch = Math.Atan2(accelY, Math.Sqrt(accelX * accelX + accelZ * accelZ)) * (180.0 / Math.PI);
                    double roll = Math.Atan2(accelX, Math.Sqrt(accelY * accelY + accelZ * accelZ)) * (180.0 / Math.PI);

                    row["pitch"] = pitch;
                    row["roll"] = roll;

                    table.Rows.Add(row);
                }

                dataGridView1.DataSource = table;

                chart1.Series.Clear();
                chart2.Series.Clear();
                chart3.Series.Clear();

                Series tempSeries = new Series("Temperature");
                tempSeries.ChartType = SeriesChartType.Line;

                Series pitchSeries = new Series("Pitch");
                pitchSeries.ChartType = SeriesChartType.Line;

                Series rollSeries = new Series("Roll");
                rollSeries.ChartType = SeriesChartType.Line;

                foreach (DataRow row in table.Rows)
                {
                    DateTime timestamp = (DateTime)row["timestamp"];
                    tempSeries.Points.AddXY(timestamp, (float)row["temperature"]);
                    pitchSeries.Points.AddXY(timestamp, (double)row["pitch"]);
                    rollSeries.Points.AddXY(timestamp, (double)row["roll"]);
                }

                chart1.Series.Add(tempSeries);
                chart2.Series.Add(pitchSeries);
                chart3.Series.Add(rollSeries);

                // Adjust X-axis labels
                AdjustXAxis(chart1, table.Rows.Count, range);
                AdjustXAxis(chart2, table.Rows.Count, range);
                AdjustXAxis(chart3, table.Rows.Count, range);
            }
            catch (HttpRequestException e)
            {
                MessageBox.Show($"Request error: {e.Message}\n{e.StackTrace}");
            }
            catch (Exception ex)
            {
                MessageBox.Show($"General error: {ex.Message}\n{ex.StackTrace}");
            }
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            string range = comboBox1.SelectedItem.ToString();
            string url = flaskServerUrl + "/get_data";

            switch (range)
            {
                case "Last 30 Seconds":
                    url += "?range=30s";
                    break;
                case "Last Minute":
                    url += "?range=minute";
                    break;
                case "Last Hour":
                    url += "?range=hour";
                    break;
                case "Last Day":
                    url += "?range=day";
                    break;
                case "Last Week":
                    url += "?range=week";
                    break;
                default:
                    url += "?range=minute";
                    break;
            }

            LoadData(url, range);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            comboBox1_SelectedIndexChanged(sender, e);
        }

        private void autoUpdateTimer_Tick(object sender, EventArgs e)
        {
            comboBox1_SelectedIndexChanged(sender, e);
        }

        private void buttonAutoUpdateOn_Click(object sender, EventArgs e)
        {
            autoUpdateTimer.Start();
            MessageBox.Show("Auto-update turned on.");
        }

        private void buttonAutoUpdateOff_Click(object sender, EventArgs e)
        {
            autoUpdateTimer.Stop();
            MessageBox.Show("Auto-update turned off.");
        }

        private void AdjustXAxis(Chart chart, int dataCount, string range)
        {
            if (dataCount == 0)
                return;

            double interval;
            switch (range)
            {
                case "Last 30 Seconds":
                    interval = 5;
                    break;
                case "Last Minute":
                    interval = 10;
                    break;
                case "Last Hour":
                    interval = 300;
                    break;
                case "Last Day":
                    interval = 3600;
                    break;
                case "Last Week":
                    interval = 86400;
                    break;
                default:
                    interval = 10;
                    break;
            }

            chart.ChartAreas[0].AxisX.Interval = interval;
            chart.ChartAreas[0].AxisX.LabelStyle.Format = "HH:mm:ss";
        }
    }
}
