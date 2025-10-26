## LiveCaptions Fork - Enhanced Readability and Extended Line Support

This is a fork of [abb128/LiveCaptions](https://github.com/abb128/LiveCaptions), modified to provide improved readability and extended line support for live captions.

### Key Changes:

*   **Persistent Text:** Captions no longer fade after 6 seconds. Text remains on screen for better readability.
*   **Increased Line Count:** The application now displays up to 10 lines of captions instead of the original 2.
*   **PipeWire Audio Server:** This fork utilizes PipeWire for audio capture instead of plain PulseAudio. This allows the autor of the fork (me!) for the use of advanced audio filters available in PipeWire.
*   **Updated Compilation Instructions:** The compilation instructions have been updated to clarify any ambiguities present in the original version.

### Tested On:

*   Arch Linux

### Presumed Compatibility:

*   Ubuntu/Debian

### Compilation Instructions:

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/Walter98garcia/Abb128-LiveCaptions-fork-by-Walter.git --recursive
    cd LiveCaptions
    ```

2.  **Install dependencies:**
    ```bash
    # For Arch Linux:
    sudo pacman -S meson ninja libadwaita
    # For Ubuntu/Debian:
    sudo apt install meson ninja libadwaita
    ```

3.  **Download and extract ONNXRuntime v1.14.1:**
    *   Download from: [https://github.com/microsoft/onnxruntime/releases/tag/v1.14.1](https://github.com/microsoft/onnxruntime/releases/tag/v1.14.1)
    *   Extract to a suitable location (e.g., `/opt/onnxruntime-linux-x64-1.14.1`).

4.  **Set environment variables:**
    ```bash
    export ONNX_ROOT=/path/to/onnxruntime-linux-x64-1.14.1/  # Replace with your actual path
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/onnxruntime-linux-x64-1.14.1/lib # Replace with your actual path
    ```

5.  **Configure the build:**
    ```bash
    meson setup builddir
    meson devenv -C builddir
    ```

6.  **Compile:**
    ```bash
    ninja -C builddir
    ```

7.  **Download the model:**
    ```bash
    wget https://april.sapples.net/april-english-dev-01110_en.april
    ```

8.  **Set the model path and run:**
    ```bash
    export APRIL_MODEL_PATH=`pwd`/april-english-dev-01110_en.april
    ./builddir/src/livecaptions
    ```

---

## Fork de LiveCaptions - Legibilidad Mejorada y Soporte Extendido de Líneas

Este es un fork de [abb128/LiveCaptions](https://github.com/abb128/LiveCaptions), modificado para proporcionar una mejor legibilidad y soporte extendido de líneas para los subtítulos en vivo.

### Cambios Clave:

*   **Texto Persistente:** Los subtítulos ya no se desvanecen después de 6 segundos. El texto permanece en la pantalla para una mejor legibilidad.
*   **Mayor Número de Líneas:** La aplicación ahora muestra hasta 10 líneas de subtítulos en lugar de las 2 originales.
*   **Servidor de Audio PipeWire:** Este fork utiliza PipeWire para la captura de audio en lugar de PulseAudio a secas. Esto permite el uso de filtros de audio avanzados disponibles en PipeWire.
*   **Instrucciones de Compilación Actualizadas:** Las instrucciones de compilación han sido actualizadas para aclarar cualquier ambigüedad presente en la versión original.

### Testeado en:

*   Arch Linux

### Compatibilidad Presunta:

*   Ubuntu/Debian

### Instrucciones de Compilación:

1.  **Clonar el repositorio:**
    ```bash
    git clone https://github.com/Walter98garcia/Abb128-LiveCaptions-fork-by-Walter.git --recursive
    cd LiveCaptions
    ```

2.  **Instalar dependencias:**
    ```bash
    # Para Arch Linux:
    sudo pacman -S meson ninja libadwaita
    # Para Ubuntu/Debian:
    sudo apt install meson ninja libadwaita
    ```

3.  **Descargar y extraer ONNXRuntime v1.14.1:**
    *   Descargar desde: [https://github.com/microsoft/onnxruntime/releases/tag/v1.14.1](https://github.com/microsoft/onnxruntime/releases/tag/v1.14.1)
    *   Extraer a una ubicación adecuada (por ejemplo, `/opt/onnxruntime-linux-x64-1.14.1`).

4.  **Establecer variables de entorno:**
    ```bash
    export ONNX_ROOT=/path/to/onnxruntime-linux-x64-1.14.1/  # Reemplaza con tu ruta real
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/onnxruntime-linux-x64-1.14.1/lib # Reemplaza con tu ruta real
    ```

5.  **Configurar la compilación:**
    ```bash
    meson setup builddir
    meson devenv -C builddir
    ```

6.  **Compilar:**
    ```bash
    ninja -C builddir
    ```

7.  **Descargar el modelo:**
    ```bash
    wget https://april.sapples.net/april-english-dev-01110_en.april
    ```

8.  **Establecer la ruta del modelo y ejecutar:**
    ```bash
    export APRIL_MODEL_PATH=`pwd`/april-english-dev-01110_en.april
    ./builddir/src/livecaptions
    ```
