'''
AzElkeyboard2Serial em 20250130
> Estabelece uma conexão serial com uma porta com
> recebe um input de teclado com azimute e elevação
> Envia o input recebido via serial
'''


import serial
arduino = serial.Serial('COM4', 9600, timeout=.5)


while True:
   msg = str(input("entre azimute em graus (-270<az>270) e elevação (0<el<90). Formato  az:el "))
   msgb = bytes(msg, "utf-8")  # converte a string em bytes..
   arduino.write(msgb) # Envia string via serial
