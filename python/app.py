import os
from flask import Flask, jsonify, render_template
import psycopg2

app = Flask(__name__)

def get_connection():
    return psycopg2.connect(
        host=os.getenv("DB_HOST", "localhost"),
        dbname=os.getenv("DB_NAME", "postgres"),
        user=os.getenv("DB_USER", "postgres"),
        password=os.getenv("DB_PASSWORD", ""),
        port=int(os.getenv("DB_PORT", "5432")),
    )

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/data")
def data():
    conn = get_connection()
    cursor = conn.cursor()

    cursor.execute("SELECT temperature, humidity, soil, timestamp FROM sensor_data ORDER BY timestamp DESC LIMIT 20")
    recent_rows = cursor.fetchall()
    recent_rows.reverse()

    cursor.close()
    conn.close()

    return jsonify({
        "recent": {
            "temp": [r[0] for r in recent_rows],
            "humi": [r[1] for r in recent_rows],
            "soil": [r[2] for r in recent_rows],
            "timestamps": [r[3].strftime("%Y-%m-%d %H:%M") for r in recent_rows],
        }
    })

if __name__ == "__main__":
    app.run(debug=True)