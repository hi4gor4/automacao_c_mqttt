'use strict'

const mqtt = require('mqtt')

const options = {
    keepalive: 30,
    port: 1883,
    protocolId: 'MQTT',
    protocolVersion: 4,
    clean: true,
    reconnectPeriod: 1000,
    connectTimeout: 30 * 1000,
    will: {
        topic: 'WillMsg',
        payload: 'Connection Closed abnormally..!',
        qos: 0,
        retain: false
    },
    rejectUnauthorized: false
};
options.clientId = "webassis"
options.username = "hiago23rangel@gmail.com"
options.password = "2314"
var status = ""
var topicMsg = ""
const topic = [
    "hiago23rangel@gmail.com/alarm1",  
    "hiago23rangel@gmail.com/alarm2",    
    "hiago23rangel@gmail.com/luz1 ",  
    "hiago23rangel@gmail.com/luz2",    
    "hiago23rangel@gmail.com/pbl2",
    "hiago23rangel@gmail.com/max",
    "hiago23rangel@gmail.com/min"
]

const client = mqtt.connect('mqtt://maqiatto.com', options)
var lastMessage = {}

client.subscribe(topic, () => {
    console.log(topic)
})

client.on('connect', () => {
    console.log(`Connected`)
    status = 'Connected'
})

client.on('reconnect', (error) => {
    console.log(`Reconnecting():`, error)
    status = 'Reconnect'
})

client.on('error', (error) => {
    console.log(`Cannot connect():`, error)
    // status = `Cannot connect():` + error
    status = "Disconnected"
})

client.on('message', (topic, payload) => {
    lastMessage[topic] = payload.toString()
})
class MqttController {

    async publishTopic({ request, response }) {
        try {
            var data = request.only(['topic', 'message'])

            client.publish(data.topic, data.message, { qos: 0, retain: false }, (error) => {
                if (error) {
                    return response.status(500).send({ status: 500, error: error })
                }
            })

            return response.status(200).send({ status: status, message: data.message })
        } catch (e) {
            return response.status(500).send({ status: 500, error: `Erro: ${e.message}` })
        }
    }

    async subscribeTopic({ request, response }) {
        try {
            var data = request.only(['topic'])
            console.log(data)
            return response.status(200).send({ status: status, message: lastMessage[data.topic]  })
        } catch (e) {
            return response.status(500).send({ status: 500, error: `Erro: ${e.message}` })
        }
    }
}

module.exports = MqttController


