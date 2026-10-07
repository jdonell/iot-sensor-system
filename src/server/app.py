import json
import logging
import time

import paho.mqtt.client as mqtt
from flask import Flask, jsonify, request

import credentials

# ---------- Inställningar ----------
BROKER_HOST = "localhost"
BROKER_PORT = 1883
TOPIC = "byggnad/rum-a/temp"
API_PORT = 5000

# ---------- Loggning: varje händelse skrivs ut med datum och tid ----------
logging.basicConfig(level=logging.INFO, format="%(asctime)s %(levelname)s %(message)s")
log = logging.getLogger("sekreteraren")

# ---------- Pärmen: senaste mätvärdet och statistik ----------
latest = None  # senaste giltiga mätvärdet, None tills något har kommit
stats = {
    "messages_received": 0,    # alla meddelanden, även ogiltiga
    "validation_errors": 0,    # meddelanden som inte följde datakontraktet
    "last_message_time": None, # när senaste meddelandet kom (Unix-tid)
}

REQUIRED_FIELDS = ["sensorId", "value", "unit"]


def validate(data):
    """Kontrollerar att meddelandet följer datakontraktet.
    Returnerar en text som beskriver felet, eller None om allt är rätt."""
    if not isinstance(data, dict):
        return "meddelandet är inte ett JSON-objekt"
    for field in REQUIRED_FIELDS:
        if field not in data:
            return f"fältet '{field}' saknas"
    if not isinstance(data["value"], (int, float)):
        return "fältet 'value' är inte ett tal"
    if not -40 <= data["value"] <= 85:
        return "värdet ligger utanför rimligt intervall (-40 till 85)"
    return None


# ---------- MQTT: funktioner som körs när något händer ----------
def on_connect(client, userdata, flags, reason_code, properties):
    if reason_code.is_failure:
        log.error("Brokern nekade anslutningen: %s", reason_code)
    else:
        log.info("Ansluten till brokern")
        client.subscribe(TOPIC)  # görs här så att den görs om vid återanslutning
        log.info("Prenumererar på %s", TOPIC)


def on_disconnect(client, userdata, flags, reason_code, properties):
    log.warning("Tappade anslutningen till brokern (%s). Försöker igen automatiskt.", reason_code)


def on_message(client, userdata, msg):
    global latest
    stats["messages_received"] += 1
    stats["last_message_time"] = time.time()
    text = msg.payload.decode(errors="replace")

    # Försök tolka JSON. Om det misslyckas kraschar vi inte, utan loggar och fortsätter.
    try:
        data = json.loads(text)
    except json.JSONDecodeError:
        stats["validation_errors"] += 1
        log.warning("Ogiltig JSON mottagen: %s", text)
        return

    error = validate(data)
    if error:
        stats["validation_errors"] += 1
        log.warning("Valideringsfel: %s (%s)", error, text)
        return

    latest = data
    log.info("Mätvärde: %s %s från %s", data["value"], data["unit"], data["sensorId"])


# ---------- API: receptionen ----------
app = Flask(__name__)


def api_key_ok():
    """Kontrollerar att anropet skickade rätt API-nyckel i headern X-API-Key."""
    return request.headers.get("X-API-Key") == credentials.API_KEY


@app.get("/api/sensors/latest")
def get_latest():
    log.info("API-anrop: GET /api/sensors/latest")
    if not api_key_ok():
        return jsonify({"error": "unauthorized", "message": "Ogiltig eller saknad API-nyckel"}), 401
    if latest is None:
        return jsonify({"error": "not_found", "message": "Inget mätvärde har tagits emot än"}), 404
    return jsonify(latest), 200


@app.get("/health")
def health():
    log.info("API-anrop: GET /health")
    last = stats["last_message_time"]
    seconds = round(time.time() - last, 1) if last else None
    return jsonify({
        "status": "ok",
        "mqtt_connected": client.is_connected(),
        "messages_received": stats["messages_received"],
        "validation_errors": stats["validation_errors"],
        "seconds_since_last_message": seconds,
    }), 200


# ---------- Start ----------
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_disconnect = on_disconnect
client.on_message = on_message
client.username_pw_set(credentials.MQTT_USER, credentials.MQTT_PASSWORD)
client.reconnect_delay_set(min_delay=1, max_delay=30)  # vänta 1-30 s mellan återanslutningsförsök

log.info("Sekreteraren startar")
client.connect_async(BROKER_HOST, BROKER_PORT)  # kraschar inte om brokern är nere, försöker igen
client.loop_start()                             # MQTT körs i bakgrunden
app.run(host="0.0.0.0", port=API_PORT)          # API:et körs i förgrunden