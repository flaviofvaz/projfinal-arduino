import tkinter as tk
from tkinter import ttk
from PIL import Image, ImageTk
import os
import math
from serial import Serial
from threading import Thread, Timer
from time import sleep

my_serial = Serial("/dev/cu.usbserial-2140", baudrate=9600, timeout=0.01)
led_positions = {}
buzzer_positions = {}
servo_positions = {}
input_positions = []
output_positions = []
connections = []
lista_cores = ["blue", "red", "green", "yellow"]
lista_inputs = ["botao", "potenciometro", "encoder", "sensor_luz"]
lista_outputs = ["led", "servo", "buzzer"]
imagens_carregadas = {}
rotary_images = {}
sensor_luz_barrinhas = {}


def serial():
    while True:
        if my_serial is not None:
            my_text = my_serial.readline().decode().strip()
            if my_text != "":
                print(my_text)
                processar_comando(my_text)

        sleep(0.1)


thread = Thread(target=serial)
thread.daemon = True
thread.start()


def processar_comando(comando):
    partes = comando.split()

    if len(partes) not in [2, 3, 4]:
        label_status.config(text="Comando inválido: Use 'tipo_componente pos_componente conectado' ou 'desconectado' ou 'ligacao pos_input pos_output feita' ou 'desfeita' ou 'tipo_output pos_output acao' ou 'servo pos_output girar angulo'", fg="red")
        return

    if len(partes) == 3 and partes[2] in ["conectado", "desconectado"]:
        tipo_componente = partes[0]
        pos_componente = int(partes[1])

        if partes[2] == "conectado":
            if tipo_componente in lista_inputs:
                desenhar_item("grid 0", pos_componente, tipo_componente)
            elif tipo_componente in lista_outputs:
                desenhar_item("grid 1", pos_componente, tipo_componente)
            else:
                label_status.config(text="Comando inválido: Tipo de componente inválido", fg="red")
        elif partes[2] == "desconectado":
            if tipo_componente in lista_inputs:
                remover_item("grid 0", pos_componente, tipo_componente)
            elif tipo_componente in lista_outputs:
                remover_item("grid 1", pos_componente, tipo_componente)
            else:
                label_status.config(text="Comando inválido: Tipo de componente inválido", fg="red")
    elif len(partes) == 4 and partes[0] == "ligacao":
        pos_input = int(partes[1])
        pos_output = int(partes[2])
        if partes[3] == "feita":
            criar_ligacao(pos_input, pos_output)
        elif partes[3] == "desfeita":
            remover_ligacao(pos_input, pos_output)
    elif len(partes) >= 3:
        tipo_componente = partes[0]
        pos_componente = int(partes[1])
        acao = partes[2]

        if tipo_componente == "led":
            if acao in ["ligado", "desligado"]:
                controlar_led(pos_componente, acao)
            else:
                label_status.config(text="Comando inválido: Ação inválida para LED", fg="red")
        elif tipo_componente == "buzzer":
            if acao in ["ligado", "desligado"]:
                controlar_buzzer(pos_componente, acao)
            else:
                label_status.config(text="Comando inválido: Ação inválida para Buzzer", fg="red")
        elif tipo_componente == "servo" and len(partes) == 4 and acao == "girar":
            try:
                angulo = int(partes[3])
                if 0 <= angulo <= 360:
                    girar_servo(pos_componente, angulo)
                else:
                    label_status.config(text="Comando inválido: Ângulo deve estar entre 0 e 360", fg="red")
            except ValueError:
                label_status.config(text="Comando inválido: Ângulo inválido", fg="red")
        elif tipo_componente == "botao" and acao in ["apertado", "solto"]:
            alterar_estado_botao(pos_componente, acao)
        elif tipo_componente == "potenciometro" and acao == "girar":
            try:
                angulo = int(partes[3])
                if 0 <= angulo <= 360:
                    girar_potenciometro(pos_componente, angulo)
                else:
                    label_status.config(text="Comando inválido: Ângulo deve estar entre 0 e 360", fg="red")
            except ValueError:
                label_status.config(text="Comando inválido: Ângulo inválido", fg="red")
        elif tipo_componente == "encoder" and acao == "girar":
            try:
                angulo = int(partes[3])
                if 0 <= angulo <= 180:
                    girar_encoder(pos_componente, angulo)
                else:
                    label_status.config(text="Comando inválido: Ângulo deve estar entre 0 e 180", fg="red")
            except ValueError:
                label_status.config(text="Comando inválido: Ângulo inválido", fg="red")
        else:
            label_status.config(text="Comando inválido: Tipo de componente ou ação inválida", fg="red")

def carregar_imagem(nome_arquivo):
    caminho_completo = os.path.join("imagens", nome_arquivo)
    imagem = Image.open(caminho_completo)
    imagem = imagem.resize((60, 60), Image.LANCZOS)
    imagem_tk = ImageTk.PhotoImage(imagem)
    return imagem_tk

def desenhar_item(grid, pos, tipo):
    x, y = (pos % 2) * 75 + 37.5, (pos // 2) * 75 + 37.5
    cor = lista_cores[pos]
    if grid == "grid 0":
        if tipo == "botao":
            img_botao = carregar_imagem("botao.png")
            canvas_inputs.create_image(x, y, image=img_botao, tags=(f"botao_{pos}"))
            input_positions.append({"tipo": tipo, "pos": pos})
            imagens_carregadas[f"botao_{pos}"] = img_botao
        elif tipo == "potenciometro":
            img_pot = carregar_imagem("potenciometro.png")
            canvas_inputs.create_image(x, y, image=img_pot, tags=(f"potenciometro_{pos}"))
            input_positions.append({"tipo": tipo, "pos": pos})
            imagens_carregadas[f"potenciometro_{pos}"] = img_pot
            desenhar_seta(canvas_inputs, x, y, pos, 0)
        elif tipo == "encoder":
            img_encoder_base = carregar_imagem("rotary_encoder_base.png")
            img_encoder_girar = Image.open(os.path.join("imagens", "rotary_encoder_girar.png")).resize((60, 60), Image.LANCZOS)
            img_encoder_girar_tk = ImageTk.PhotoImage(img_encoder_girar)
            canvas_inputs.create_image(x, y, image=img_encoder_base, tags=(f"encoder_base_{pos}"))
            canvas_inputs.create_image(x, y, image=img_encoder_girar_tk, tags=(f"encoder_girar_{pos}"))
            input_positions.append({"tipo": tipo, "pos": pos})
            rotary_images[f"encoder_girar_{pos}"] = img_encoder_girar
            imagens_carregadas[f"encoder_girar_{pos}"] = img_encoder_girar_tk
        elif tipo == "sensor_luz":
            img_sensor_luz = carregar_imagem("sensor_luz.png")
            canvas_inputs.create_image(x, y, image=img_sensor_luz, tags=(f"sensor_luz_{pos}"))
            barra = canvas_inputs.create_rectangle(x-30, y+30, x-20, y+30, fill="black", tags=(f"sensor_luz_barra_{pos}"))
            input_positions.append({"tipo": tipo, "pos": pos})
            sensor_luz_barrinhas[f"sensor_luz_barra_{pos}"] = barra
            imagens_carregadas[f"sensor_luz_{pos}"] = img_sensor_luz

        circle_x, circle_y = x - 37.5 + 5, y + 37.5 - 5
        canvas_inputs.create_oval(circle_x-4, circle_y-4, circle_x+4, circle_y+4, fill=cor, outline="black", tags=(f"circulo_{pos}"))

    elif grid == "grid 1":
        if tipo == "led":
            img_led_off = carregar_imagem("led_off.png")
            led = canvas_outputs.create_image(x, y, image=img_led_off, tags=(f"led_{pos}"))
            led_positions[pos] = led
            output_positions.append({"tipo": tipo, "pos": pos})
            imagens_carregadas[f"led_{pos}"] = img_led_off
        elif tipo == "servo":
            img_servo_base = carregar_imagem("servo_base.png")
            img_servo_girar = Image.open(os.path.join("imagens", "servo_girar.png")).resize((60, 60), Image.LANCZOS)
            img_servo_girar_tk = ImageTk.PhotoImage(img_servo_girar)
            canvas_outputs.create_image(x, y, image=img_servo_base, tags=(f"servo_base_{pos}"))
            canvas_outputs.create_image(x, y, image=img_servo_girar_tk, tags=(f"servo_girar_{pos}"))
            output_positions.append({"tipo": tipo, "pos": pos})
            servo_positions[f"servo_girar_{pos}"] = img_servo_girar
            imagens_carregadas[f"servo_girar_{pos}"] = img_servo_girar_tk
            imagens_carregadas[f"servo_base_{pos}"] = img_servo_base
        elif tipo == "buzzer":
            img_buzzer_off = carregar_imagem("buzzer_desligado.png")
            buzzer = canvas_outputs.create_image(x, y, image=img_buzzer_off, tags=(f"buzzer_{pos}"))
            buzzer_positions[pos] = buzzer
            output_positions.append({"tipo": tipo, "pos": pos})
            imagens_carregadas[f"buzzer_{pos}"] = img_buzzer_off

    label_status.config(text=f"{tipo} posicionado na posição {pos} no {grid}", fg="green")

def remover_item(grid, pos, tipo):
    if grid == "grid 0":
        if tipo == "botao":
            tags = f"botao_{pos}"
        elif tipo == "potenciometro":
            tags = f"potenciometro_{pos}"
        elif tipo == "encoder":
            tags = f"encoder_{pos}"
        elif tipo == "sensor_luz":
            tags = f"sensor_luz_{pos}"
            barra = f"sensor_luz_barra_{pos}"
            canvas_inputs.delete(barra)

        canvas_inputs.delete(tags)
        input_positions.remove({"tipo": tipo, "pos": pos})
        imagens_carregadas.pop(f"{tipo}_{pos}", None)
        canvas_inputs.delete(f"circulo_{pos}")

    elif grid == "grid 1":
        if tipo == "led":
            tags = f"led_{pos}"
        elif tipo == "servo":
            tags = [f"servo_base_{pos}", f"servo_girar_{pos}"]
        elif tipo == "buzzer":
            tags = f"buzzer_{pos}"

        if isinstance(tags, list):
            for tag in tags:
                canvas_outputs.delete(tag)
        else:
            canvas_outputs.delete(tags)

        output_positions.remove({"tipo": tipo, "pos": pos})
        imagens_carregadas.pop(f"{tipo}_{pos}", None)

    label_status.config(text=f"{tipo} removido da posição {pos} no {grid}", fg="green")

def desenhar_seta(canvas, x, y, pos, angulo):
    raio = 20
    angulo_rad = math.radians(angulo)
    end_x = x + raio * math.cos(angulo_rad)
    end_y = y - raio * math.sin(angulo_rad)
    seta_id = f"seta_{pos}"
    items = canvas.find_withtag(seta_id)
    for item in items:
        canvas.delete(item)
    canvas.create_line(x, y, end_x, end_y, arrow=tk.LAST, fill="red", tags=(seta_id))

def girar_potenciometro(pos, angulo):
    x, y = (pos % 2) * 75 + 37.5, (pos // 2) * 75 + 37.5
    desenhar_seta(canvas_inputs, x, y, pos, angulo)
    label_status.config(text=f"Potenciômetro na posição {pos} girado para {angulo} graus.", fg="green")

def girar_encoder(pos, angulo):
    x, y = (pos % 2) * 75 + 37.5, (pos // 2) * 75 + 37.5
    img_encoder_girar = rotary_images[f"encoder_girar_{pos}"]
    img_rotacionada = img_encoder_girar.rotate(angulo, resample=Image.BICUBIC)
    img_rotacionada_tk = ImageTk.PhotoImage(img_rotacionada)
    canvas_inputs.create_image(x, y, image=img_rotacionada_tk, tags=(f"encoder_girar_{pos}", f"encoder_{pos}"), anchor=tk.CENTER)
    imagens_carregadas[f"encoder_girar_{pos}"] = img_rotacionada_tk
    label_status.config(text=f"Encoder na posição {pos} girado para {angulo} graus.", fg="green")

def ajustar_nivel_sensor_luz(pos, nivel):
    barra_id = f"sensor_luz_barra_{pos}"
    barra = sensor_luz_barrinhas[barra_id]
    x0, y0, x1, y1 = canvas_inputs.coords(barra)
    altura_total = 60
    nova_altura = altura_total * (nivel / 100.0)
    canvas_inputs.coords(barra, x0, y1 - nova_altura, x1, y1)
    cor = "black" if nivel == 0 else "yellow"
    canvas_inputs.itemconfig(barra, fill=cor)
    label_status.config(text=f"Sensor de luz na posição {pos} ajustado para {nivel}% de luz.", fg="green")

def criar_ligacao(pos_input, pos_output):
    for input in input_positions:
        if input["pos"] == pos_input:
            for output in output_positions:
                if output["pos"] == pos_output:
                    connections.append({
                        "tipo_input": input["tipo"],
                        "pos_input": pos_input,
                        "tipo_output": output["tipo"],
                        "pos_output": pos_output
                    })
                    desenhar_circulo_no_output(input["pos"], output["pos"])
                    desenhar_linha_ligacao(pos_input, pos_output)
                    label_status.config(text=f"Input na posição {pos_input} conectado ao output na posição {pos_output}", fg="green")
                    return
    label_status.config(text="Comando inválido: Posições de input ou output não encontradas", fg="red")

def remover_ligacao(pos_input, pos_output):
    for connection in connections:
        if connection["pos_input"] == pos_input and connection["pos_output"] == pos_output:
            connections.remove(connection)
            label_status.config(text=f"Ligação entre input {pos_input} e output {pos_output} removida", fg="green")
            remover_linha_ligacao(pos_input, pos_output)
            return
    label_status.config(text="Comando inválido: Ligação não encontrada", fg="red")

def desenhar_circulo_no_output(pos_input, pos_output):
    cor = lista_cores[pos_input]
    x, y = (pos_output % 2) * 75 + 37.5, (pos_output // 2) * 75 + 37.5
    circle_x, circle_y = x - 37.5 + 5, y + 37.5 - 5
    canvas_outputs.create_oval(circle_x-4, circle_y-4, circle_x+4, circle_y+4, fill=cor, outline="black", tags=(f"circulo_output_{pos_output}"))

def desenhar_linha_ligacao(pos_input, pos_output):
    x1, y1 = (pos_input % 2) * 75 + 37.5, (pos_input // 2) * 75 + 37.5
    x2, y2 = (pos_output % 2) * 75 + 37.5, (pos_output // 2) * 75 + 37.5
    canvas_inputs.create_line(x1, y1, x2, y2, fill="black", tags=(f"linha_{pos_input}_{pos_output}"))
    canvas_outputs.create_line(x1, y1, x2, y2, fill="black", tags=(f"linha_{pos_input}_{pos_output}"))

def remover_linha_ligacao(pos_input, pos_output):
    tags = f"linha_{pos_input}_{pos_output}"
    canvas_inputs.delete(tags)
    canvas_outputs.delete(tags)
    canvas_outputs.delete(f"circulo_output_{pos_output}")

def controlar_led(pos, acao):
    if any(conn for conn in connections if conn["pos_output"] == pos and conn["tipo_output"] == "led" and conn["tipo_input"] == "botao"):
        led = led_positions[pos]
        img_led_on = carregar_imagem("led_on.png")
        img_led_off = carregar_imagem("led_off.png")
        if acao == "ligado":
            canvas_outputs.itemconfig(led, image=img_led_on)
            imagens_carregadas[f"led_{pos}"] = img_led_on
            label_status.config(text=f"LED na posição {pos} ligado.", fg="green")
        elif acao == "desligado":
            canvas_outputs.itemconfig(led, image=img_led_off)
            imagens_carregadas[f"led_{pos}"] = img_led_off
            label_status.config(text=f"LED na posição {pos} desligado.", fg="green")
    else:
        label_status.config(text="Comando inválido: LED não está ligado a um botão", fg="red")

def controlar_buzzer(pos, acao):
    if any(conn for conn in connections if conn["pos_output"] == pos and conn["tipo_output"] == "buzzer" and conn["tipo_input"] == "botao"):
        buzzer = buzzer_positions[pos]
        img_buzzer_on = carregar_imagem("buzzer_ligado.png")
        img_buzzer_off = carregar_imagem("buzzer_desligado.png")
        if acao == "ligado":
            canvas_outputs.itemconfig(buzzer, image=img_buzzer_on)
            imagens_carregadas[f"buzzer_{pos}"] = img_buzzer_on
            label_status.config(text=f"Buzzer na posição {pos} ligado.", fg="green")
        elif acao == "desligado":
            canvas_outputs.itemconfig(buzzer, image=img_buzzer_off)
            imagens_carregadas[f"buzzer_{pos}"] = img_buzzer_off
            label_status.config(text=f"Buzzer na posição {pos} desligado.", fg="green")
    else:
        label_status.config(text="Comando inválido: Buzzer não está ligado a um botão", fg="red")

def girar_servo(pos, angulo):
    if any(conn for conn in connections if conn["pos_output"] == pos and conn["tipo_output"] == "servo" and conn["tipo_input"] in ["potenciometro", "encoder"]):
        x, y = (pos % 2) * 75 + 37.5, (pos // 2) * 75 + 37.5
        img_servo_girar = servo_positions[f"servo_girar_{pos}"]
        img_rotacionada = img_servo_girar.rotate(angulo, resample=Image.BICUBIC)
        img_rotacionada_tk = ImageTk.PhotoImage(img_rotacionada)
        canvas_outputs.create_image(x, y, image=img_rotacionada_tk, tags=(f"servo_girar_{pos}", f"servo_{pos}"), anchor=tk.CENTER)
        imagens_carregadas[f"servo_girar_{pos}"] = img_rotacionada_tk
        label_status.config(text=f"Servo na posição {pos} girado para {angulo} graus.", fg="green")
    else:
        label_status.config(text="Comando inválido: Servo não está ligado a um potenciômetro ou encoder", fg="red")

def alterar_estado_botao(pos, estado):
    botao_id = f"botao_{pos}"
    botao = canvas_inputs.find_withtag(botao_id)[0]
    if estado == "apertado":
        img_botao_apertado = carregar_imagem("botao_apertado.png")
        canvas_inputs.itemconfig(botao, image=img_botao_apertado)
        imagens_carregadas[f"botao_{pos}"] = img_botao_apertado
    elif estado == "solto":
        img_botao = carregar_imagem("botao.png")
        canvas_inputs.itemconfig(botao, image=img_botao)
        imagens_carregadas[f"botao_{pos}"] = img_botao

def mostrar_conexoes(tipo, pos):
    top = tk.Toplevel(janela)
    title = f"Conexões do {tipo} na posição {pos}"
    top.title(title)
    top.geometry("200x150")
    top.configure(bg="#E3A9A9")

    label = tk.Label(top, text="Conexões:", bg="#E3A9A9", fg="#FFFFFF")
    label.pack(pady=5)

    conexao_encontrada = False

    for connection in connections:
        if connection["tipo_input"] == tipo and connection["pos_input"] == pos:
            tk.Label(top, text=f"{tipo} na posição {pos} conectado a {connection['tipo_output']} na posição {connection['pos_output']}", bg="#E3A9A9", fg="#FFFFFF").pack(pady=2)
            conexao_encontrada = True

        elif connection["tipo_output"] == tipo and connection["pos_output"] == pos:
            tk.Label(top, text=f"{tipo} na posição {pos} conectado a {connection['tipo_input']} na posição {connection['pos_input']}", bg="#E3A9A9", fg="#FFFFFF").pack(pady=2)
            conexao_encontrada = True

    if not conexao_encontrada:
        tk.Label(top, text="Nenhuma conexão encontrada.", bg="#E3A9A9", fg="#FFFFFF").pack(pady=2)

janela = tk.Tk()
janela.title("Simulador de Dispositivos")
janela.geometry("450x400")
janela.configure(bg="#d9ead3")
entrada_comando = ttk.Entry(janela, width=30)
entrada_comando.grid(row=0, column=0, padx=20, pady=10)

style = ttk.Style()
style.configure("TButton", background="#6aa84f", foreground="white", font=("Arial", 10, "bold"))
botao_enviar = ttk.Button(janela, text="Enviar", command=processar_comando, style="TButton")
botao_enviar.grid(row=0, column=1, padx=10, pady=10)

label_status = tk.Label(janela, text="Digite um comando e clique em Enviar", fg="black", bg="#d9ead3")
label_status.grid(row=1, column=0, columnspan=2, padx=20, pady=5)

label_inputs = tk.Label(janela, text="Inputs", font=("Arial", 10, "bold"), bg="#d9ead3", fg="#6aa84f")
label_inputs.grid(row=2, column=0, padx=20, pady=5)

label_outputs = tk.Label(janela, text="Outputs", font=("Arial", 10, "bold"), bg="#d9ead3", fg="#6aa84f")
label_outputs.grid(row=2, column=1, padx=20, pady=5)

canvas_inputs = tk.Canvas(janela, width=150, height=150, bg="white", highlightthickness=2, highlightbackground="#6aa84f")
canvas_inputs.grid(row=3, column=0, padx=20, pady=5)

canvas_outputs = tk.Canvas(janela, width=150, height=150, bg="white", highlightthickness=2, highlightbackground="#6aa84f")
canvas_outputs.grid(row=3, column=1, padx=20, pady=5)

for i in range(1, 2):
    canvas_inputs.create_line(i*75, 0, i*75, 150)
    canvas_inputs.create_line(0, i*75, 150, i*75)

    canvas_outputs.create_line(i*75, 0, i*75, 150)
    canvas_outputs.create_line(0, i*75, 150, i*75)

janela.mainloop()
