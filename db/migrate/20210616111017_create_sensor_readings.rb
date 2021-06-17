class CreateSensorReadings < ActiveRecord::Migration[6.1]
  def change
    create_table :sensor_readings do |t|

      t.timestamps
      t.integer :temperature
      t.integer :humidity
    end
  end
end
