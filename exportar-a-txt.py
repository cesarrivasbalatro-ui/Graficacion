from pathlib import Path

# Define la ruta principal
ruta_base = Path(r"C:\openGL\U4T1")

# Busca todos los archivos .h y .cpp de forma recursiva
for archivo in ruta_base.rglob("*"):
  if archivo.is_file() and archivo.suffix.lower() in [".h", ".cpp"]:
    # Define la ruta del nuevo archivo con .txt al final
    archivo_destino = archivo.with_suffix(archivo.suffix + ".txt")

    # Lee el contenido del original y lo escribe en el nuevo archivo
    try:
      contenido = archivo.read_text(encoding="utf-8", errors="ignore")
      archivo_destino.write_text(contenido, encoding="utf-8")
      print(f"Copiado: {archivo.name} -> {archivo_destino.name}")
    except Exception as e:
      print(f"Error procesando {archivo.name}: {e}")

print("\n¡Proceso finalizado!")