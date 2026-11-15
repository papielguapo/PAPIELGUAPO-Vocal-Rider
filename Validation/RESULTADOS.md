# Validación — Vocal Rider K 1.0.0

Versión final con FIX a la izquierda de RIDE/OUT, selector giratorio K-12 / K-14 / K-20 a la izquierda del medidor y objetivo fijo a 0 dB K. Sin parámetro TARGET, slider TRIM ni HOLD.

- Compilación Release MSVC x64: correcta, VST3 y Standalone.
- Pruebas DSP: todas aprobadas. Incluyen convergencia de los tres modos a 0 K, límites de subida y bajada, silencio, bypass, FIX encendido/apagado, recuperación de estado y mono. El test comprueba también ganancia enlazada estéreo y salida finita.
- pluginval 1.0.4: strictness 10, semilla 121420, SUCCESS, salida 0. Sin excluir pruebas de GUI. Tasas 44100 / 48000 / 96000 Hz; bloques 64 / 128 / 256 / 512 / 1024. Ejecutado sobre el VST3 incluido en Binaries.
- Interfaz: captura renderizada y revisada en Preview.png.

Los ensayos DSP son offline con señales sintéticas; no sustituyen una escucha de voces reales ni certifican compatibilidad con todos los DAW. La nivelación tiene tiempos de respuesta y límites de ganancia: 0 dB K es su objetivo medio, no una lectura instantánea garantizada. No limita picos.

Logs íntegros: DSP.txt y pluginval-level10.txt.

## Instalación y segunda auditoría

Instalado en `C:/Program Files/Common Files/VST3/Vocal Rider K.vst3`. SHA-256 del binario instalado verificado contra la entrega: `2C3CE92E2D076B18D0086D4785A9F091215003729F3FCCFA7CE9B1B54CCFFE3A`.

Segunda ejecución de pluginval nivel 10 sobre la copia instalada: SUCCESS, salida 0, semilla 121421, con pruebas de GUI y las tasas/bloques por defecto. Log: pluginval-installed-level10.txt.
