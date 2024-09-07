from flask import Flask, render_template_string
import serial
import threading
import time

app = Flask(__name__)
received_data = "Waiting for data..."
nodes_connected = 0
server_address = "http://127.0.0.1:5000/"

ser = serial.Serial('COM3', 115200, timeout=1)

def read_serial():
    global received_data, nodes_connected
    while True:
        try:
            if ser.in_waiting > 0:
                received_data = ser.readline().decode('utf-8').rstrip()
                nodes_connected = 1
            else:
                nodes_connected = 0
            time.sleep(1)
        except Exception as e:
            print(f"Error reading serial data: {e}")

@app.route('/')
def index():
    html = '''
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>VEEConnect Dashboard</title>
        <link rel="icon" href="{{ url_for('static', filename='favicon.png') }}" type="image/png">
        <link href="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css" rel="stylesheet">
        <link href="https://fonts.googleapis.com/css2?family=Roboto:wght@400;700&family=Quicksand:wght@300;500&display=swap" rel="stylesheet">
        <style>
            body {
                font-family: 'Quicksand', sans-serif;
                background: url('{{ url_for('static', filename='background.png') }}') no-repeat center center fixed;
                background-size: cover;
                color: #d32f2f; /* Dark red */
            }
            .container {
                margin-top: 50px;
            }
            .header {
                margin-bottom: 30px;
                text-align: center;
            }
            .header img {
                max-width: 200px;
                margin-bottom: 20px;
                border-radius: 10px;
            }
            .header h1, .header p {
                color: #d32f2f; /* Dark red */
                font-weight: bold;
            }
            .dashboard {
                background: rgba(255, 255, 255, 0.9);
                border-radius: 15px;
                box-shadow: 0 0 15px rgba(0, 0, 0, 0.2);
                padding: 30px;
                text-align: center;
                max-width: 700px;
                margin: 0 auto;
            }
            .info-card, .data-card {
                font-size: 20px;
                font-weight: bold;
                color: #d32f2f; /* Dark red */
                border: 2px solid #d32f2f; /* Dark red border */
                padding: 20px;
                border-radius: 15px;
                margin-bottom: 20px;
                background-color: #fff;
                box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
            }
            .data-card {
                background-color: #ffe5e5; /* Light red background */
            }
            .footer {
                margin-top: 30px;
                padding: 15px;
                background-color: #d32f2f; /* Dark red */
                color: white;
                text-align: center;
                border-radius: 0 0 15px 15px;
            }
            .footer p {
                margin: 0;
                font-weight: bold;
            }
            .btn-custom {
                background-color: #d32f2f;
                color: white;
                border: none;
                border-radius: 25px;
                padding: 10px 20px;
                font-size: 18px;
                margin-top: 20px;
                font-weight: bold;
            }
            .btn-custom:hover {
                background-color: #c62828;
                text-decoration: none;
            }
        </style>
    </head>
    <body>
        <div class="container">
            <div class="header">
                <img src="{{ url_for('static', filename='logo.png') }}" alt="VEEConnect Logo">
                <h1 class="display-4">VEEConnect Dashboard</h1>
                <p class="lead">Monitor your ESP32 device's live data.</p>
            </div>
            <div class="dashboard">
                <div class="info-card">
                    <p><strong>Nodes Connected:</strong> {{ nodes_connected }}</p>
                    <p><strong>Server Address:</strong> {{ server_address }}</p>
                    <p><strong>Area of the Server:</strong> Chennai</p>
                </div>
                <div class="data-card">
                    <p><strong>Serial Data:</strong></p>
                    <p>{{ received_data }}</p>
                </div>
                <a href="#" class="btn btn-custom">Refresh Data</a>
            </div>
            <div>
            </div>
        </div>

        <script src="https://code.jquery.com/jquery-3.5.1.slim.min.js"></script>
        <script src="https://cdn.jsdelivr.net/npm/@popperjs/core@2.9.3/dist/umd/popper.min.js"></script>
        <script src="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js"></script>
    </body>
    </html>
    '''
    return render_template_string(html,
                                  nodes_connected=nodes_connected,
                                  server_address=server_address,
                                  received_data=received_data)

if __name__ == '__main__':
    serial_thread = threading.Thread(target=read_serial)
    serial_thread.daemon = True
    serial_thread.start()

    app.run(host='0.0.0.0', port=5000)

