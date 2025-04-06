from flask import Flask, request, jsonify
from datetime import datetime
import mysql.connector
from mysql.connector import Error
import logging

app = Flask(__name__)

# Database connection configuration
db_config = {
    'host': 'localhost',
    'user': 'test',
    'password': 'DB_PASSWORD',
    'database': 'SensorData'
}


# Configure logging
logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)

def create_connection():
    try:
        connection = mysql.connector.connect(**db_config)
        if connection.is_connected():
            logger.info("Connected to MariaDB database")
            return connection
    except Error as e:
        logger.error(f"Error: {e}")
        return None

@app.route('/add_data', methods=['POST'])
def add_data():
    data = request.json
    temperature = data.get('temperature')
    mic = data.get('mic')
    accelX = data.get('accelX')
    accelY = data.get('accelY')
    accelZ = data.get('accelZ')
    gyroX = data.get('gyroX')
    gyroY = data.get('gyroY')
    gyroZ = data.get('gyroZ')

    connection = create_connection()
    if connection:
        try:
            cursor = connection.cursor()
            query = """INSERT INTO SensorReadings 
                       (temperature, mic, accelX, accelY, accelZ, gyroX, gyroY, gyroZ) 
                       VALUES (%s, %s, %s, %s, %s, %s, %s, %s)"""
            cursor.execute(query, (temperature, mic, accelX, accelY, accelZ, gyroX, gyroY, gyroZ))
            connection.commit()
            cursor.close()
            connection.close()
            logger.info("Data added successfully")
            return jsonify({'message': 'Data added successfully!'}), 200
        except Error as e:
            logger.error(f"Error while inserting data: {e}")
            return jsonify({'message': 'Failed to insert data'}), 500
    else:
        return jsonify({'message': 'Failed to connect to database'}), 500

@app.route('/get_data', methods=['GET'])
def get_data():
    range_param = request.args.get('range', default='hour')
    query = "SELECT * FROM SensorReadings WHERE timestamp > NOW() - INTERVAL 1 HOUR ORDER BY timestamp DESC"
    if range_param == 'day':
        query = "SELECT * FROM SensorReadings WHERE timestamp > NOW() - INTERVAL 1 DAY ORDER BY timestamp DESC"
    elif range_param == 'week':
        query = "SELECT * FROM SensorReadings WHERE timestamp > NOW() - INTERVAL 1 WEEK ORDER BY timestamp DESC"
    elif range_param == 'minute':
        query = "SELECT * FROM SensorReadings WHERE timestamp > NOW() - INTERVAL 1 MINUTE ORDER BY timestamp DESC"
    elif range_param == '30seconds':
        query = "SELECT * FROM SensorReadings WHERE timestamp > NOW() - INTERVAL 30 SECOND ORDER BY timestamp DESC"

    logger.debug(f"Executing query: {query}")
    logger.debug(f"Current server time: {datetime.now()}")

    connection = create_connection()
    if connection:
        try:
            cursor = connection.cursor(dictionary=True)
            cursor.execute(query)
            rows = cursor.fetchall()
            cursor.close()
            connection.close()
            for row in rows:
                row['timestamp'] = row['timestamp'].isoformat()
            logger.debug(f"Rows fetched: {rows}")
            return jsonify(rows), 200
        except Error as e:
            logger.error(f"Error while fetching data: {e}")
            return jsonify({'message': 'Failed to fetch data'}), 500
    else:
        return jsonify({'message': 'Failed to connect to database'}), 500


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
