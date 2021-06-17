require "mqtt"

module ApplicationHelper
  class MQTTSubscriber
    def run
      Thread.new do
        username = 'hello-world-application'
        password = 'PASSWORD_HERE'

        application_id = 'hello-world-application'

        device_id = 'thingsuno'

        topic_base = "v3/#{application_id}@ttn/devices/#{device_id}"

        client = MQTT::Client.connect("mqtt://#{username}:#{password}@eu1.cloud.thethings.network:1883")

        client.subscribe("#{topic_base}/up")

        client.get do |_topic, message|
          parsed_message = JSON.parse message
          data = parsed_message['uplink_message']['decoded_payload']

          SensorReading.create(temperature: data['temp'], humidity: data['humidity'])
        end
      end
    end
  end
end
