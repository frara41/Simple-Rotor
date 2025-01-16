'''
GPreader3c3 em 20250114


Versão para testes e refinamento da conexão com o  Gpredict
versão com extração de valores inteiros de coordenadas confirmado

===
conecta com GPredict via TCPIP
Implementa subset do hamlib para controle de rotor
Recebe do GPredic coordenadas de apontamento do rotor
Verifica o Delta entre o azimute recebido  e anterior. Só envia por serial se o Delta for maior que o valor de ref (Dang).
Envia coordenadas via serial para o controlador do rotor

===
Esta versão está conseguindo mandar azimutes positivos e elevação para a serial (trava com azimutes negativos), então exige configuração do Gpredic para enviar dados no formato 0 > 360 graus:

Faltando:
Implementar leitura serial de feeedback da posição do rotor
Implementar rotina de inicialização da posição do rotor
Implementar rotina para enviar azm e el quando a variação de az for pequena  (< dang), mas houver variação significativa de Elevação
Realizar otimização do código
Melhorar comentários e documentação

===
OBS:
Para iniciar o rastreio:

01 > conectar a interface do rotor na porta COM3 (ou outra, conforme o código) do computador. a interface levará o rotor para a posição 0,0
02 > Apontar manualmente o rotor para o norte geográfico (0) >> calibração de direçâo
03 > rodar este código
04 > no Gpredict, antenna control, selecionar o satelite alvo e iniciar o track
05 > No Gpredict, antenna control, selecionar o device (Teste2) e engajar



'''
import socket
import re
import serial





HOST = "localhost"  # (127.0.0.1 )
PORT = 4533  # Port defined in GPredict


arduino = serial.Serial('COM3', 9600, timeout=.5)




azx = 0  # guarda valor anterior do azimute enviado pelo Gpredict
azini = 0 #  guarda valor de azimute inicial medido, para avaliação do sentido de rotação
Dflag = 1  # indica  se o sentido de rotação já foi obtido
flag = 1  #  indica  se o azimute inicial (azini) já foi definido
azflag = 1 # indica  se houve mudança significativa de azimute (maior que Dang graus) >> azflag = 1, enviar azimute, na condição "abs(az-azx) >= Dang graus" ; azflag = 0, não envia azimute
Dang = 2 # Valor de delta em graus para que um novo valor de azimute seja enviado para a serial (define resolução de atualização do rotor)
sent = 0 # define o sentido de rotação do rotor, a partir de duas leituras iniciais de azimute >> sent = 1 se passagem é em sentido horário, e -1 se sentido anti-horário
azmod= 0 # azimute modificado para formato -270 >> 0 >> 270



class Rotor_serial(object): 
   '''
   rotor serial genérico(RtrS)
   '''
   az = 0
   el = 0


   print('azimute e elevação ', (az, el))


   def set_pos(RtrS, az, el):
       print('enviado: %d,%d' % (az, el))
       RtrS.az = az
       RtrS.el = el


   def get_pos(RtrS):
       print('recebido: %d,%d' % (RtrS.az, RtrS.el))
       return (RtrS.az, RtrS.el)


   def stop(RtrS):
       print("enviado: Stop")



#=============================================================================================================
# O trecho de código a seguir estabelece a conexão com o Gpredict, e recebe os dados de azimute e elevação

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
   s.bind((HOST, PORT))
   s.listen()
   print("servidor esperando")
   conn, addr = s.accept()
   rotor = Rotor_serial
   with conn:
       print(f"Connected by {addr}   client")


       while True:
           Ordr = conn.recv(1024)
           Ordr = Ordr.strip()
           if Ordr == '':
               print('sei lá') # Nunca printou nada. Não sei se é necessário


           
           if Ordr == b'\\dump_state':
               conn.send(b'RPRT 0\n')


          


          
           if Ordr == b'q':
               RtrS.close_client(conn)


         
           if Ordr == b'S':
          
               conn.send(b'RPRT 0\n')


         
           if Ordr == b'p':
               # pos = RtrS.rotor.get_pos() # Leitura da posição do rotor, que dever ser lida da serial = ainda não implementado
               pos = 0, 0 # atribui a posição do rotor como azm = 0, el. = 0. caso não receba um resultado deste comando, o Gpredict trava
               if not pos:
                   # print('-2-> RPRT -6') impressões para acompanhar o processamento da aplicação. Não devem ser necessários na versão final
                   conn.send(b'RPRT -6\n')
               else:
                   azr, elr = pos
                 
                   conn.send(b'%.6f\n%.6f\n' % (azr, elr))


           #=============================================================================================================
           # O trecho de código a seguir extrai as coordenadas do pacote enviado pelo Gpredict via conexão tcp/ip
           
           Ordrs = Ordr.decode("utf-8")  # não sei se é necessário manter isso
           match = re.match(r"^P\s+([\d,]+)\s+([\d,]+)$", Ordrs)  # versão para notação decimal do sistema com ","
           # match = re.match(r'^P\s+([\d.]+)\s+([\d.]+)$', Ordrs) # versão para notação decimal do sistema com "."


           if match:
               az = int(match.groups()[0][:-3])
               el = int(match.groups()[1][:-3])
               print('recebido GP: az = %s el = %s ' % (az, el))
               # Série de impressões para acompanhar o processamento da aplicação. Não devem ser necessários na versão final
               print('Dflag  = %s  ' % (Dflag))
               print('flag  = %s  ' % (flag))
               print('sent = %s  ' % (sent))
               print('azflag  = %s  ' % (azflag))
               #print('Dang  = %s  ' % (Dang)) 
               deltaAZ = abs(az - azx)
               print('deltaAZ  = %s  ' % (deltaAZ))
               if (Dflag == 1):  # avalia direção de variação do rotor
                   if (flag == 1):
                       azini = az
                       flag = 0
                   if (abs(az - azini) > 1):
                       sent = int((az - azini)/(abs(az - azini))) # sent = 1 se passagem é em sentido horário, e -1 se sentido anti-horário
                       Dflag = 0

# =============================================================================================================
# O trecho de código a seguir faz a conversão da medida de  azimute de um formato 0>> 360 para o formato -270 >> 0 >> 270, 
# conforme o sentido da passagem e o quadrante de AOS do satélite

               if (sent == 1): # se o movimento for horário:
                   
                   if(0<= azini<=90):
                       azmod = az
                
                   if(90< azini<= 180):
                       azmod = az -360
                   
                   if(180< azini<= 270):                     
                        if (180 < az < 360):
                            azmod = az-360
                        if (0<= az <=180):
                            azmod = az
                
                   if (270< azini <= 360):
                       if (180 < az < 360):
                            azmod = az-360
                       if (0<= az <=180):
                            azmod = az
                
                
               if (sent == -1): # se o movimento for anti-horário:
                   
                   if(0<= azini<=90):
                       if (180 < az < 360):
                           azmod = az-360
                       if (0<= az <=180):
                           azmod = az
                       
                
                   if(90< azini<= 180):
                       if (180 < az < 360):
                           azmod = az-360
                       if (0<= az <=180):
                           azmod = az
                                              
                   
                   if(180< azini<= 270):                                           
                            azmod = az
                
                   if (270< azini <= 360):                    
                            azmod = az-360
                       
# =============================================================================================================

# O trecho de código a seguir gera o string para envio de azimute  e elevação via serial, e faz um print do valor enviado no monitor  

               if (azflag == 1):  # confere se houve mudança significativa de azimute (maior que Dang graus)
                   Adtin = ("%s:%s" % (azmod,el)) # gera string  no formato de exportação
                   print('envio serial: %s ' % (Adtin)) #imprime variável para conferência
                   msgb = bytes(Adtin, "utf-8")  # converte  string  para binário para envio serial
                   arduino.write(msgb)           # envia  string  para a serial
# =============================================================================================================

               if (deltaAZ < Dang):  # confere se houve mudança significativa de azimute (maior que Dang graus)
                       azflag = 0  # modifica o flag para que não haja envio de azimute


               if (deltaAZ >= Dang):
                      if(azx != 0):
                           azflag = 1  # modifica o flag para enviar azimute, na condição "abs(az-azx) >= Dang graus"
                      else:
                          azflag = 0
                      azx = az  # grava novo valor de azimute


               try:
                   az = int(float(az))
                   el = int(float(el))
               except:
                   # print('-4-> RPRT -8 (could not parse)') impressões para acompanhar o processamento da aplicação. Não devem ser necessários na versão final
                   conn.send(b'RPRT -8\n')


             #  print('-5-> RPRT 0') impressões para acompanhar o processamento da aplicação. Não devem ser necessários na versão final
               conn.send(b'RPRT 0\n')


           if not Ordr:
               break
       # conn.sendall(b'%.6f\n%.6f\n' % (az,el))
