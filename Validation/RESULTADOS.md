# Vocal Rider K 1.1.0 — Auditoría

Versión Windows x64 con escalas impresas alrededor de las perillas y SAFE en el hueco derecho del medidor. FIX conserva el objetivo automático de 0 dB K. No hay slider TRIM.

## Compilación e instalación

Release MSVC x64: VST3 y Standalone compilados correctamente. Instalado en `C:/Program Files/Common Files/VST3/Vocal Rider K.vst3`. SHA-256 del binario instalado idéntico al entregado: `31948430687155da1287454417ef556aeda7bbb2f9c9b96840863130f366af43`.

## Pluginval

pluginval 1.0.4, nivel de exigencia 10, semilla 121430, ejecutado sobre la copia instalada de la versión 1.1.0: **SUCCESS, salida 0**. Sin excluir pruebas de GUI. Tasas 44100/48000/96000 Hz y bloques 64/128/256/512/1024. Incluye automatización y fuzzing de parámetros. Log íntegro en pluginval-installed-level10.txt.

## Audio

Pruebas DSP aprobadas: convergencia K-12/K-14/K-20, límites de boost/cut, silencio, bypass, FIX, mono/estéreo enlazado, recuperación de estado y ausencia de parámetros TARGET/TRIM.

SAFE: detector FIR 8x de 65 taps, ganancia enlazada, anticipación de 3 ms más 96 muestras de margen y detector. Latencia fija de 240 muestras / 5 ms a 48 kHz, reportada al host e idéntica con SAFE apagado. La prueba de impulso verifica la latencia y la conservación exacta de la muestra con SAFE/FIX apagados. Recuperación aproximada de 80 ms.

24 mediciones independientes con interpolación 32x/129 taps, a 44.1/48/96 kHz: senoide con picos entre muestras, tonos muy agudos (incluido 0.499 × sampleRate), impulsos, onda cuadrada, multitono y ruido. Ningún caso excedió −1 dBTP; máximo observado **-1.23627 dBTP**. Datos completos en true-peak.txt. SAFE emplea un margen interno conservador de 1.2 dB: puede actuar antes de llegar al techo mostrado. No se debe interpretar una prueba finita como garantía matemática sobre cualquier señal posible.

## Interfaz y límites de la auditoría

Preview.png renderizada y revisada. Escalas situadas según la posición real de los parámetros. SAFE y FIX arrancan encendidos; LED ámbar indica reducción de picos. El medidor principal sigue siendo RMS K.

Las pruebas son offline con señales sintéticas y un host de validación; no se ha comprobado abriendo una sesión real en FL Studio. El nivel medio de 0 dB K está sujeto a límites, umbral y tiempos de respuesta; SAFE puede reducirlo para dar prioridad a los picos. FL Studio debe volver a escanear/reabrir el plugin para cargar el binario actualizado.
