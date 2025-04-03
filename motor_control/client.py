#
# client.py
#
# Client for communication with PIC in a more friendly way (over UART). Adapated from ch28.py
#
# Max Hughes
# ME 333
# Feb 2025
#

import serial
import matplotlib.pyplot as plt
import genref
from statistics import mean 

modes = ['IDLE', 'PWM', 'ITEST', 'HOLD', 'TRACK'] # only for printing

def process_buffer(buf):
    data = [row.split(b" ") for row in buf.strip(b'b\r\n').split(b",")]
    data = [[float(item) for item in row ] for row in data[0:-1]]
    data = [list(row) for row in zip(*data)]
    return data

def send_buffer(traj):
    ser.write(('b\n').encode())
    for traj_point in traj:
        ser.write(("{:.3f}\n".format(traj_point)).encode())
    ser.write(('s\n').encode())

def plot_data(data):
    t = range(len(data[0])) # time array
    score = mean([abs(target - measured) for target, measured in zip(data[0], data [1])])
    plt.plot(t,data[0],'r*-',t,data[1],'b*-')
    plt.title('Score = {:.3f}'.format(score))
    plt.ylabel('value')
    plt.xlabel('sample')
    plt.legend(["Desired Value", "Actual Value"])
    plt.show()



print('Opening port: ')
ser = serial.Serial('//./COM10',230400)
print(ser.name)

has_quit = False
# menu loop
while not has_quit:
    print('PIC32 MOTOR DRIVER INTERFACE')

    # display the menu options
    print('\ta: Read current sensor (ADC counts) \tb: Read current sensor (mA)')
    print('\tc: Read encoder (counts) \t\td: Read encoder (deg)')
    print('\te: Reset encoder \t\t\tf: Set PWM (-100 to 100)')
    print('\tg: Set current gains \t\t\th: Get current gains')
    print('\ti: Set position gains \t\t\tj: Get position gains')
    print('\tk: Test current control \t\tl: Go to angle (deg)')
    print('\tm: Load step trajectory \t\tn: Load cubic trajectory')
    print('\to: Execute trajectory \t\t\tp: Unpower the motor')
    print('\tq: Quit client \t\t\t\tr: Get mode')

    # read the user's choice
    selection = input('\nENTER COMMAND: ')
    selection_endline = selection+'\n'
    print()
     
    # send the command to the PIC32
    ser.write(selection_endline.encode()); # .encode() turns the string into a char array

    if (selection == 'a'): # current counts
        ser_str = ser.read_until(b'\n') 
        #print(int(ser_str))
        print('Current sensor implementation does not use ADC and has no counts.')

    elif (selection == 'b'): # current mA
        ser_str = ser.read_until(b'\n')
        print("The measured current is {}mA".format(float(ser_str)))

    elif (selection == 'c'): # encoder counts
        ser_str = ser.read_until(b'\n')
        print("The measured encoder angle is {} counts".format(int(ser_str)))

    elif (selection == 'd'): # encoder deg
        ser_str = ser.read_until(b'\n')
        print("The measured encoder angle is {} degrees".format(float(ser_str)))

    elif (selection == 'e'): # encoder tare
        print('The encoder angle has been reset')

    elif (selection == 'f'): # PWM input
        input_str = input('Input a desired PWM value from -100 to 100: ')
        ser.write((input_str +'\n').encode()); # send the number
    
    elif (selection == 'g'): # current gains input (0.00112 0.0003)
        input_str = input('Input float values or Kp and Ki for the current controller (Kp Ki Eint_max): ')
        ser.write((input_str +'\n').encode()); # send the number

    elif (selection == 'h'): # current gains output
        ser_str = ser.read_until(b'\n')
        gains = [float(num) for num in ser_str.split()]
        print('The current controller gains are: Kp = {}, Ki = {}, Eint_max = {}'.format(gains[0], gains[1], gains[2]))

    elif (selection == 'i'): # pos gains input
        input_str = input('Input float values for Kp, Ki, and Kd for the position controller (Kp Ki Kd Eint_max): ')
        ser.write((input_str +'\n').encode()); # send the number
        #gains = [float(num) for num inp input_str.split()]

    elif (selection == 'j'): # pos gains output
        ser_str = ser.read_until(b'\n')
        gains = [float(num) for num in ser_str.split()]
        print('The position controller gains are: Kp = {}, Ki = {}, Kd = {}, Eint_max = {}'.format(gains[0], gains[1], gains[2], gains[3]))

    elif (selection == 'k'): # ITEST
        print('Preforming current test by changing mode...')
        ser_str = ser.read_until(b'\n')
        data = process_buffer(ser_str)
        plot_data(data)

    elif (selection == 'l'): # set angle
        input_str = input('Input a target angle in degrees: ')
        ser.write((input_str +'\n').encode()); # send the number

    elif (selection == 'm'): # load step traj
        traj = genref.genRef('step')
        print("Sending trajectory...")
        send_buffer(traj)
        print("Trajectory Sent..")

    elif (selection == 'n'): # load cubic traj
        traj = genref.genRef('cubic')
        print("Sending trajectory...")
        send_buffer(traj)
        print("Trajectory Sent")

    elif (selection == 'o'): # execute traj
        print('Executing trajectory by changing mode...')
        ser_str = ser.read_until(b'\n')
        data = process_buffer(ser_str)
        plot_data(data)

    elif (selection == 'p'): # unpower
        print('Unpowering motor...')

    elif (selection == 'r'): # get mode
        mode = int(ser.read_until(b'\n'))
        print('Current mode is {}.'.format(modes[mode]))

    elif (selection == 'q'): #quit
        print('Exiting client')
        has_quit = True; # exit client
        # be sure to close the port
        ser.close()

    else:
        print('Invalid Selection ' + selection_endline)
    print()

