import serial
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import csv
from collections import deque

raw_data_string = ""  # create a string to hold the array data from serial port

# set port parameters
str_port = 'COM8'
baud = 115200


class ReadSerial:
    def __init__(self, port, maxLen):
        self.ser = serial.Serial(port, baud)

        # self.altitude_data = deque([0.0000]*maxLen) # altitude
        self.accx_data = deque([0.0000]*maxLen) # acceleration in x
        self.accy_data = deque([0.0000]*maxLen) # acceleration in y
        self.accz_data = deque([0.0000]*maxLen) # acceleration in z. important for apogee
        self.maxLen = maxLen

    def add(self, data):
        """
        insert data into deque
        :param data:
        :return:
        """
        assert(len(data) == 4) # we need 4 values from the serial port
        # self.add_to_deque(self.altitude_data, data[0])
        self.add_to_deque(self.accx_data, data[1])
        self.add_to_deque(self.accy_data, data[2])
        self.add_to_deque(self.accz_data, data[3])

    def add_to_deque(self, buf, val):
        """
        Add data to deque from the left side
        :param buf:
        :param val:
        :return:
        """
        buf.pop() # remove the oldest element
        buf.appendleft(val) # most recent values appear at the left of the graph

    def store_data(self, frameNum, ax, ay, az):
        """
        Update the csv file
        :return:
        """
        try:
            # create a xsv file to write data to
            flight_log = open('flight_data.csv', 'a', newline='')
            writer_obj = csv.writer(flight_log) # create a csv write object
            writer_obj.writerow(['Altitude', 'ax', 'ay', 'az'])

            line = self.ser.readline().decode('Ascii') # convert from binary data to ascii
            data = [float(value) for value in line.split(',')]
            # remove \r\n carriage return on the last data element
            # data[3] = data[3][:4]
            # print(len(data))

            if len(data) == 4:
                self.add(data)
                # altitude.set_data(range(self.maxLen), self.altitude_data)
                ax.set_data(range(self.maxLen), self.accx_data) # update graph with new values
                ay.set_data(range(self.maxLen), self.accy_data)
                az.set_data(range(self.maxLen), self.accz_data)

            # write data to csv file priority 1
            writer_obj.writerow(data)

            # print(data)
        except:
            pass

        flight_log.close()
        return ax, ay, az

    def clean_serial(self):
        """
        flush, clean and close the serial port
        close open files
        :return:
        """

        self.ser.flush()
        self.ser.close()


def main():
    # plot parameters
    maxLen = 100

    # create plot object
    # create read serial object
    ser = ReadSerial(str_port, maxLen)

    # set up animation
    fig = plt.figure()
    axi = plt.axes(xlim=(0, maxLen), ylim=(-20, 20))

    # altitude, = axi.plot([], [], '-b', label='Altitude')
    accx, = axi.plot([], [], '-r', label='acc-x')
    accy, = axi.plot([], [], '-g', label='acc-y')
    accz, = axi.plot([], [], '-m', label='acc-z')

    leg = axi.legend(loc='upper left', frameon=True)
    axi.set_title("Flight raw data")

    # set tick frequency
    # plt.xticks(np.arange(-10, altitude, 0.1))

    anim = animation.FuncAnimation(fig, ser.store_data, fargs=(accx, accy, accz), interval=20)

    # show plot
    plt.show()

    # close serial port
    ser.clean_serial()


if __name__ == '__main__':
    main()
