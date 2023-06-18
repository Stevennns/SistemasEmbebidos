import os

def imprimir_estructura_directorio(directorio, nivel=0, carpeta_omitir=None):
    for item in os.listdir(directorio):
        ruta = os.path.join(directorio, item)
        ruta_normalizada = os.path.normpath(ruta)
        carpeta_omitir_normalizada = os.path.normpath(carpeta_omitir) if carpeta_omitir else None
        if os.path.isdir(ruta) and ruta_normalizada != carpeta_omitir_normalizada:
            #print(ruta_normalizada, "*",carpeta_omitir_normalizada)
            print("  " * nivel + "|-- " + item + "/")
            imprimir_estructura_directorio(ruta, nivel + 1, carpeta_omitir)
        else:
            print("  " * nivel + "|-- " + item)

# Ejemplo de uso:
directorio_raiz = "sample_project"
carpeta_a_omitir = "sample_project/build"
imprimir_estructura_directorio(directorio_raiz, carpeta_omitir=carpeta_a_omitir)
print("")
