import serial
# import numpy as np
import csv

raw_data_string = ""  # create a string to hold the array data from serial port

# set port parameters
str_port = 'COM8'
baud = 115200


class ReadSerial:
    def __init__(self, port):
        self.ser = serial.Serial(port, baud)

    def store_data(self):
        """
        Update the csv file
        :return:
        """

        # create a xsv file to write data to
        flight_log = open('flight_data.csv', 'a', newline='')
        writer_obj = csv.writer(flight_log) # create a csv write object
        writer_obj.writerow(['Altitude', 'ax', 'ay', 'az'])

        while 1:
            line = self.ser.readline().decode('Ascii') # convert from binary data to ascii
            data = [value for value in line.split(',')]

            # remove \r\n carriage return on the last data element
            data[3] = data[3][:4]

            # write data to csv file
            writer_obj.writerow(data)

            print(data)

        # close csv file
        flight_log.close()

    def clean_serial(self):
        """
        flush, clean and close the serial port
        :return:
        """

        self.ser.flush()
        self.ser.close()


def main():
    # create read serial object
    ser = ReadSerial(str_port)

    print("Reading flight data...")

    ser.store_data()

    # close serial port
    ser.clean_serial()


if __name__ == '__main__':
    main()
