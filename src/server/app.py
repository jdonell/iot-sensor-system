import paho.mqtt.client as mqtt
import credentials

def on_connect(client, userdata, flags, reason_code, properties):
    print("Svar från brokern:", reason_code)
    client.subscribe("byggnad/rum-a/temp")

def on_message(client, userdata, msg):
    print("Meddelande på", msg.topic, ":", msg.payload.decode())

print("Sekreteraren startar")
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message
client.username_pw_set(credentials.MQTT_USER, credentials.MQTT_PASSWORD)
client.connect("localhost", 1883)
client.loop_forever()