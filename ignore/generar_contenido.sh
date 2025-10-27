#!/bin/bash

# ==============================================================================
#           GENERADOR DE CONTENIDO DEL PROYECTO LiveCaptions-Walter
# Versión: 4.3 (Exclusiones e inclusiones específicas actualizadas)
# ==============================================================================

# --- Configuración ---
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
PROJECT_ROOT=$(realpath "$SCRIPT_DIR/..")
OUTPUT_FILE="$SCRIPT_DIR/contenido_proyecto.txt"
SEPARATOR_LINE="================================================================================"

# --- Listas de Filtrado ---
INCLUDED_PATTERNS=(
    "c" "h" "ui" "xml" "css" "meson.build" "in" "svg" "md"
    "walter" "txt" "nix" "lock" "sh" "POTFILES" "LINGUAS" "README"
)

# Directorios y archivos que NO queremos procesar en el contenido.
EXCLUDED_DIRS=(
    "./.git"
    "./.github"
    "./build"
    "./builddir"
)
EXCLUDED_FILES=(
    "gschemas.compiled"
    "livecaptions-fork.deb"
    "COPYING"
    "./ignore/contenido_proyecto.txt"
    "./april-english-dev-01110_en.april"
)

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
    # Se excluyen build, builddir, .git y .github, pero no se excluye la carpeta 'ignore' ni 'subprojects'
    tree_exclude_pattern=$(printf "%s|" "${EXCLUDED_DIRS[@]}" | sed 's/\.\///g' | sed 's/|$//')
    tree -I "$tree_exclude_pattern"
    echo ""
    echo "$SEPARATOR_LINE"
    echo -e "\n\n"
} >> "$OUTPUT_FILE"
echo "   ... estructura añadida."

# --- 2. Contenido de los archivos ---
echo "2. Procesando contenido de los archivos..."

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

PRIMARY_FILE="Instrucciones.walter"
if [ -f "$PRIMARY_FILE" ]; then
    append_file_content "$PRIMARY_FILE"
fi

# --- Inclusiones explícitas ---
append_file_content "./ignore/generar_contenido.sh"
append_file_content "./.gitignore"
append_file_content "./.gitmodules"
append_file_content "./subprojects/april-asr"

# --- Búsqueda y procesamiento general ---
grep_pattern=""
for pattern in "${INCLUDED_PATTERNS[@]}"; do
    if [[ "$pattern" == "meson.build" || "$pattern" == "README" || "$pattern" == "LINGUAS" || "$pattern" == "POTFILES" ]]; then
        grep_pattern+="$pattern$|"
    else
        grep_pattern+="\\.$pattern$|"
    fi
done
grep_pattern="(${grep_pattern%|})"

exclude_pattern=""
for item in "${EXCLUDED_DIRS[@]}" "${EXCLUDED_FILES[@]}"; do
    escaped_item=$(echo "$item" | sed 's/\./\\./g')
    exclude_pattern+="$escaped_item|"
done
exclude_pattern="^(${exclude_pattern%|})"

find . -type f | grep -E "$grep_pattern" | grep -vE "$exclude_pattern" | sort | while read -r file_path; do
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
