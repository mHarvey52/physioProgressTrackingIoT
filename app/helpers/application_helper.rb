require "mqtt"

module ApplicationHelper
  class MQTTSubscriber
    def run
      Thread.new do
        username = 'physio-magnetometer-proj'
        password = 'QV5NN3BGXY7SQVGH7QMLOJST4BZYMHKGRF4XUGA'
        #API Key: QV5NN3BGXY7SQVGH7QMLOJST4BZYMHKGRF4XUGA

        application_id = 'physio-magnetometer-proj'

        device_id = '0004a30b001c1cc5'

        topic_base = "v3/#{application_id}@ttn/devices/#{device_id}"

        client = MQTT::Client.connect("mqtt://#{username}:#{password}@eu1.cloud.thethings.network:1883")

        client.subscribe("#{topic_base}/up")

        client.get do |_topic, message|
          parsed_message = JSON.parse message
          data = parsed_message['uplink_message']['decoded_payload']

          SensorReading.create(exercise1: data['ex1'], exercise2: data['ex2'], exercise3: data['ex3'])
        end
      end
    end
  end
end
