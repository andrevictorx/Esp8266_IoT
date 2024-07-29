import paho.mqtt.client as mqtt
import time
import json
import mysql.connector
from datetime import datetime

def on_connect(client, userdata, flags, rc):
	if rc==0:
		print("Cliente conectado ao Broker!\n")
		try:
			client.subscribe("Treinamento/ets/Projeto", qos=0)
			print("Cliente inscrito no tópico!\n")
		except:
			print("Não foi possível inscrever o cliente no tópico\n")
	else:
		print("Cliente não conectado ao Broker.\n")


def on_disconnect(client, userdata, rc):
	if rc != 0:
		print("Cliente disconectado do Broker inesperadamente.\n")	


def on_message(client, userdata, msg):
    #print(msg.payload)
    payload = str(msg.payload.decode('utf-8'))
    msgTopic = str(msg.topic)
    print(payload + "\n")

    # separa parâmetros do payload
    jsonPayload = json.loads(payload)
    id_arduino = jsonPayload['id']
    temperatura = jsonPayload['temp']
    umidade = jsonPayload['umid']
    gas = jsonPayload['gas']
    movimento = jsonPayload['mov']
    data_hora = str(datetime.now().strftime("%Y/%m/%d %H:%M:%S"))

    #conecta com o banco de dados e insere os dados
    con = mysql.connector.connect(user='root', password='*********',
                              host='localhost',
                              port='3306',
                              database='',
                              use_pure=True)
    cursor = con.cursor()

    insert_query = """INSERT INTO esp (id_arduino,temperatura, umidade, gas, movimento) VALUES ({}, {}, {}, {},'{}')""".format(id_arduino, temperatura, umidade, gas, movimento)
    print(insert_query)
    cursor.execute(insert_query)
    con.commit()
    print("Dados inseridos no banco com sucesso!")
    
    
        
broker_address='broker.hivemq.com'
broker_port=1883

client = mqtt.Client()
client.on_connect = on_connect
client.on_disconnect = on_disconnect
client.on_message = on_message
print("Conectando ao Broker")
client.connect(broker_address, broker_port)
time.sleep(0.5)
client.loop_forever()

