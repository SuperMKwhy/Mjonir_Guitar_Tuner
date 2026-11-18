import socket

IP = "0.0.0.0"
PORT = 12345
OUTPUT_FILE = "output.raw"  # Raw 16-bit PCM data

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((IP, PORT))
    s.listen(1)
    print(f"Listening on {IP}:{PORT}")
    conn, addr = s.accept()
    with conn:
        print(f"Connected by {addr}")
        with open(OUTPUT_FILE, "wb") as f:
            while True:
                data = conn.recv(1024)
                if not data:
                    break
                f.write(data)
