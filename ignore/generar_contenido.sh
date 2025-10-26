#!/bin/bash

# ==============================================================================
#           GENERADOR DE CONTENIDO DEL PROYECTO LiveCaptions-Walter
# Versión: 4.0 (Método simple y directo. A prueba de fallos.)
# ==============================================================================

# --- Configuración ---
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
PROJECT_ROOT=$(realpath "$SCRIPT_DIR/..")
OUTPUT_FILE="$SCRIPT_DIR/contenido_proyecto.txt"
SEPARATOR_LINE="================================================================================"

# --- Listas de Filtrado ---
# Extensiones que SÍ queremos.
INCLUDED_EXTENSIONS=(
    "c" "h" "ui" "xml" "css" "meson.build" "in" "svg" "md"
    "walter" "txt" "nix" "lock" "sh" "POTFILES" "LINGUAS" "README"
)
# Carpetas y archivos que NO queremos.
EXCLUDED_DIRS=("./.git" "./ignore" "./subprojects")
EXCLUDED_FILES=("gschemas.compiled" "livecaptions-fork.deb" "COPYING")

# --- Medición de tiempo ---
start_time=$(date +%s.%N)

# --- Inicio del Proceso ---
echo -e "\n$SEPARATOR_LINE"
echo "  GENERANDO CONTENIDO DEL PROYECTO: LiveCaptions-Walter"
echo "  Archivo de Salida:       $OUTPUT_FILE"
echo -e "$SEPARATOR_LINE\n"

cd "$PROJECT_ROOT" || { echo "ERROR: No se pudo acceder a $PROJECT_ROOT"; exit 1; }
> "$OUTPUT_FILE"

# --- 1. Estructura de directorios ---
echo "1. Generando estructura de directorios..."
{
    echo "$SEPARATOR_LINE"
    echo "           ESTRUCTURA DE DIRECTORIOS Y ARCHIVOS (TREE)"
    echo "$SEPARATOR_LINE"
    echo ""
    tree -I '.git|ignore|subprojects'
    echo ""
    echo "$SEPARATOR_LINE"
    echo -e "\n\n"
} >> "$OUTPUT_FILE"
echo "   ... estructura añadida."

# --- 2. Contenido de archivos ---
echo "2. Procesando contenido de los archivos..."

# Función para añadir el contenido de un archivo
append_file_content() {
    local file_path="$1"
    echo "   - Añadiendo: $file_path"
    {
        echo "$SEPARATOR_LINE"
        echo "--- Archivo: $file_path"
        echo "$SEPARATOR_LINE"
        echo ""
        if [ -r "$file_path" ] && [ -s "$file_path" ]; then
            cat "$file_path"
        elif [ ! -s "$file_path" ]; then
            echo "### ARCHIVO VACÍO ###"
        else
            echo "### ERROR: NO SE PUDO LEER EL ARCHIVO ###"
        fi
        echo -e "\n\n"
    } >> "$OUTPUT_FILE"
}

# --- Procesamiento Prioritario ---
PRIMARY_FILE="Instrucciones.walter"
if [ -f "$PRIMARY_FILE" ]; then
    append_file_content "$PRIMARY_FILE"
fi

# --- Búsqueda y procesamiento de todos los demás archivos ---
# Este es el método más simple:
# 1. `find` crea la lista completa de archivos.
# 2. `grep` filtra esa lista para quedarnos solo con lo que queremos.
# 3. El bucle `while` procesa cada archivo de la lista final.

# Paso 1: Generar lista de extensiones para grep
# Crea una cadena como: (\.c|\.h|\.ui|meson\.build|README)$
grep_pattern=$(printf "\\.%s$|" "${INCLUDED_EXTENSIONS[@]}")
grep_pattern="(${grep_pattern%|})" # Elimina el último '|'

# Paso 2: Generar exclusiones para grep
# Crea una cadena como: ./.git/|./ignore/|./subprojects/|gschemas.compiled|COPYING
exclude_pattern=$(printf "%s/|%s|" "${EXCLUDED_DIRS[@]}" "${EXCLUDED_FILES[@]}")
exclude_pattern="^(${exclude_pattern%|})"

# Paso 3: Ejecutar y procesar
find . -type f | grep -E "$grep_pattern" | grep -vE "$exclude_pattern" | sort | while read -r file_path; do
    # Nos aseguramos de no procesar de nuevo el archivo prioritario
    if [[ "$(basename "$file_path")" != "$PRIMARY_FILE" ]]; then
        clean_path="${file_path#./}"
        append_file_content "$clean_path"
    fi
done

echo "   ... archivos procesados."

# --- Finalización ---
end_time=$(date +%s.%N)
duration=$(echo "$end_time - $start_time" | bc)

echo "$SEPARATOR_LINE" >> "$OUTPUT_FILE"
echo "Generación completada en ${duration} segundos." >> "$OUTPUT_FILE"
echo "$SEPARATOR_LINE" >> "$OUTPUT_FILE"

echo -e "\n$SEPARATOR_LINE"
echo "  Proceso completado con éxito."
echo "  Tiempo total: ${duration} segundos."
echo -e "$SEPARATOR_LINE"
