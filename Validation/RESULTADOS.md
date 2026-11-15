# Vocal Rider K 1.2.1 — Auditoría

Versión Windows x64 con RANGE simétrico, SENSITIVITY y sidechain musical opcional sin controles nuevos en la skin. Sin sidechain, FIX busca 0 dB K. Se conservan SAFE y las escalas impresas; no hay TRIM.

## Compilación e instalación

Release MSVC x64: VST3 y Standalone compilados correctamente. SHA-256 del binario VST3 incluido en el paquete: `32fdc180fc6561b8c87e358ca84d04ab2cef7478c562f3306e9d2232aeddbe6e`.

## Pluginval

pluginval 1.0.4, nivel de exigencia 10, semilla 121441, ejecutado sobre el VST3 incluido en el paquete de la versión 1.2.1: **SUCCESS, salida 0**. Sin excluir pruebas de GUI. Tasas 44100/48000/96000 Hz y bloques 64/128/256/512/1024. Incluye automatización y fuzzing de parámetros. Log íntegro en pluginval-level10.txt.

## Audio

Pruebas DSP aprobadas: convergencia K-12/K-14/K-20, límites de boost/cut, silencio, bypass, FIX, mono/estéreo enlazado, recuperación de estado y ausencia de parámetros TARGET/TRIM.

SAFE: detector FIR 8x de 65 taps, ganancia enlazada, anticipación de 3 ms más 96 muestras de margen y detector. Latencia fija de 240 muestras / 5 ms a 48 kHz, reportada al host e idéntica con SAFE apagado. La prueba de impulso verifica la latencia y la conservación exacta de la muestra con SAFE/FIX apagados. Recuperación aproximada de 80 ms.

24 mediciones independientes con interpolación 32x/129 taps, a 44.1/48/96 kHz: senoide con picos entre muestras, tonos muy agudos (incluido 0.499 × sampleRate), impulsos, onda cuadrada, multitono y ruido. Ningún caso excedió −1 dBTP; máximo observado **-1.23627 dBTP**. Datos completos en true-peak.txt. SAFE emplea un margen interno conservador de 1.2 dB: puede actuar antes de llegar al techo mostrado. No se debe interpretar una prueba finita como garantía matemática sobre cualquier señal posible.

## RANGE, SENSITIVITY y sidechain

Pruebas aprobadas para rango simétrico y rango cero, sensibilidad baja con tolerancia de ±3 dB, sensibilidad alta con cero K exacto y migración de estados antiguos. La sensibilidad no modifica el tiempo RIDE.

Bus auxiliar mono/estéreo opcional probado: aprendizaje inicial, seguimiento de +4/−4 dB del instrumental, vuelta al cero K ante silencio/desconexión, sin latencia extra y sin sobrescribir las muestras del bus auxiliar. El seguimiento queda limitado a ±6 dB alrededor del cero K. El objetivo depende del instrumental cuando está conectado. No se ha comprobado el enrutado dentro de una sesión real de FL Studio.

## Cambio visual 1.2.1

SAFE pasa a la izquierda de RIDE/OUT; FIX pasa a la derecha. SAFE incorpora LED morado lateral y aro morado en el interruptor como en la referencia. No se cambia el DSP ni los IDs de los botones.

## Interfaz y límites de la auditoría

Preview.png renderizada y revisada. Escalas situadas según la posición real de los parámetros. SAFE y FIX arrancan encendidos; LED morado lateral indica SAFE activo y aumenta de brillo al limitar. El medidor principal sigue siendo RMS K.

Las pruebas son offline con señales sintéticas y un host de validación; no se ha comprobado abriendo una sesión real en FL Studio. El nivel medio de 0 dB K está sujeto a límites, umbral y tiempos de respuesta; SAFE puede reducirlo para dar prioridad a los picos. FL Studio debe volver a escanear/reabrir el plugin para cargar el binario actualizado.
