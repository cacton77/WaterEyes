# System
import sys
import csv
import time
import datetime
from os.path import exists
# Web scraping
import requests
from bs4 import BeautifulSoup
# Data Plotting
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button
from matplotlib.animation import FuncAnimation


class PlantClient(object):
    def __init__(self):
        # Making a GET request
        self.ip = 'http://192.168.2.194/'
        try:
            r = requests.get(self.ip, timeout=10)
        except:
            print("Failed to connect to server.")
            exit()

        # Open long-term storage
        self.data_rows = []
        if exists('./data.csv'):
            with open('./data.csv', 'r') as f:
                reader = csv.reader(f)
                for row in reader:
                    self.data_rows.append(row)
        else:
            self.data_rows.append(['Time', 'Plant 1',
                                   'Plant 2', 'Plant 3', 'Plant 4'])

        # Data storage arrays
        self.x = [0]
        self.y1 = [0]
        self.y2 = [0]
        self.y3 = [0]
        self.y4 = [0]

        self.storage = 60  # Number of datapoints to store in memory
        # plt.axis([0, self.storage - 1, 0, 4095])

        # Water bounds
        self.y1_l = 0.55
        self.y2_l = 0.5
        self.y3_l = 0.45
        self.y4_l = 0.4
        # Create figure and line objects
        # print(plt.style.available)
        plt.style.use('seaborn-muted')
        plt.style.use('dark_background')
        # Create data plots
        self.fig = plt.figure(figsize=(6, 4))
        self.ln1, = plt.plot(
            self.x, self.y1, color='xkcd:kelly green', linestyle='-')
        self.ln2, = plt.plot(
            self.x, self.y2, color='xkcd:dark teal', linestyle='-')
        self.ln3, = plt.plot(
            self.x, self.y3, color='xkcd:seafoam green', linestyle='-')
        self.ln4, = plt.plot(
            self.x, self.y4, color='xkcd:blue green', linestyle='-')
        # Create lower bound plots
        self.ln1_l, = plt.plot(
            self.x, [self.y1_l] * len(self.y1),
            color='xkcd:kelly green', linestyle='--')
        self.ln2_l, = plt.plot(
            self.x, [self.y2_l] * len(self.y2),
            color='xkcd:dark teal', linestyle='--')
        self.ln3_l, = plt.plot(
            self.x, [self.y3_l] * len(self.y3),
            color='xkcd:seafoam green', linestyle='--')
        self.ln4_l, = plt.plot(
            self.x, [self.y4_l] * len(self.y4),
            color='xkcd:blue green', linestyle='--')

        plt.axis([0, self.storage - 1, 0, 1])
        plt.gcf().autofmt_xdate()
        self.fig.legend(
            ['Plant 1', 'Plant 2', 'Plant 3', 'Plant 4'],
            loc='center right')
        self.fig.canvas.mpl_connect('key_press_event', self.on_press)

    def run(self):
        animation = FuncAnimation(self.fig, self.update_plot, interval=1000)
        plt.grid()
        plt.show()

    def get_data(self):
        try:
            r = requests.get(self.ip, timeout=10)
        except:
            print("Server connection lost.")
            exit()

        # Parsing the HTML
        soup = BeautifulSoup(r.content, 'html.parser')

        table = soup.find('table')
        head = table.find_all('th')
        dataset = table.find_all('td')
        data = [0.]*len(dataset)
        for i in range(len(dataset)):
            data[i] = float(dataset[i].text)
        # Get current time
        t = time.localtime()
        self.data_rows.append([time.asctime(t)] + data)

        return data

    def update_plot(self, data):
        data = self.get_data()

        self.x.append(self.x[-1]+1)
        # self.x.append(datetime.datetime.now())
        self.y1.append(data[0])
        self.y2.append(data[1])
        self.y3.append(data[2])
        self.y4.append(data[3])

        if len(self.x) > self.storage:
            self.x.pop(0)
            self.y1.pop(0)
            self.y2.pop(0)
            self.y3.pop(0)
            self.y4.pop(0)

        self.ln1.set_data(self.x, self.y1)
        self.ln2.set_data(self.x, self.y2)
        self.ln3.set_data(self.x, self.y3)
        self.ln4.set_data(self.x, self.y4)

        self.ln1_l.set_data(self.x, [self.y1_l]*len(self.x))
        self.ln2_l.set_data(self.x, [self.y2_l]*len(self.x))
        self.ln3_l.set_data(self.x, [self.y3_l]*len(self.x))
        self.ln4_l.set_data(self.x, [self.y4_l]*len(self.x))

        plt.axis([self.x[0], self.x[-1], 0, 1])
        return self.ln1, self.ln2, self.ln3, self.ln4

    def on_press(self, event):
        print('Saving data.')
        sys.stdout.flush()
        if event.key == 'x':
            self.quit_client()

    def quit_client(self):
        with open('data.csv', 'w') as f:
            writer = csv.writer(f)
            writer.writerows(self.data_rows)
        exit()


if __name__ == "__main__":
    client = PlantClient()
    client.run()
