import re
import sys

def check_interleaved_logs(file_path: str = "log.txt") -> bool:
    """
    Lee un archivo de logs y verifica que los timestamps estén en orden cronológico.
    Retorna False si encuentra una línea intercalada (timestamp menor al anterior).
    """
    # Expresión regular para extraer el timestamp al inicio de la línea
    log_pattern = re.compile(r"^(\d+)\s+\d+\s+.+$")
    last_timestamp = 0
    
    try:
        with open(file_path, "r", encoding="utf-8") as f:
            for line_number, line in enumerate(f, start=1):
                line = line.strip()
                if not line:
                    continue
                
                match = log_pattern.match(line)
                if not match:
                    print(f"[!] Línea {line_number} ignorada por formato inválido: '{line}'")
                    continue
                
                current_timestamp = int(match.group(1))
                
                # Comprobación estricta de orden cronológico
                if current_timestamp < last_timestamp:
                    print(f"\n❌ ERROR: Línea intercalada detectada en la línea {line_number}.")
                    print(f"   -> Timestamp anterior : {last_timestamp} ms")
                    print(f"   -> Timestamp actual   : {current_timestamp} ms")
                    print(f"   -> Contenido del log  : '{line}'\n")
                    return False
                
                last_timestamp = current_timestamp
                
        print("\n✅ El log está perfectamente ordenado. No hay líneas intercaladas.")
        return True

    except FileNotFoundError:
        print(f"Error: No se encontró el archivo '{file_path}'.")
        sys.exit(1)

if __name__ == "__main__":
    # Puedes cambiar "log.txt" por la ruta de tu archivo, o pasarlo como argumento
    log_file = sys.argv[1] if len(sys.argv) > 1 else "log.txt"
    check_interleaved_logs(log_file)
