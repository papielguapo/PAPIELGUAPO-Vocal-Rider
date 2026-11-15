# Vocal Rider K 1.3.0 — Auditoría

Versión Windows x64 con RANGE simétrico, SENSITIVITY y sidechain musical opcional sin controles nuevos en la skin. Sin sidechain, FIX busca TARGET ajustable. La escala K es solo de medición. Se conservan SAFE y las escalas impresas; no hay TRIM.

## Compilación e instalación

Release MSVC x64: VST3 y Standalone compilados correctamente. Instalado en C:/Program Files/Common Files/VST3/Vocal Rider K.vst3. SHA-256 del binario instalado idéntico al del paquete: `1c8d6c8a225934b74deff8fb82b2c3a55c5f0958e31a66f06f25fc4b79ddaaf3`.

## Pluginval

pluginval 1.0.4, nivel de exigencia 10, semilla 121450, ejecutado sobre la copia instalada en Common Files/VST3 de la versión 1.3.0: **SUCCESS, salida 0**. Sin excluir pruebas de GUI. Tasas 44100/48000/96000 Hz y bloques 64/128/256/512/1024. Incluye automatización y fuzzing de parámetros. Log íntegro en pluginval-level10.txt.

## Audio

Pruebas DSP aprobadas: convergencia a distintos niveles TARGET e independencia de la escala K, límites de boost/cut, silencio, bypass, FIX, mono/estéreo enlazado, recuperación de estado y presencia de TARGET y ausencia de TRIM.

SAFE: detector FIR 8x de 65 taps, ganancia enlazada, anticipación de 3 ms más 96 muestras de margen y detector. Latencia fija de 240 muestras / 5 ms a 48 kHz, reportada al host e idéntica con SAFE apagado. La prueba de impulso verifica la latencia y la conservación exacta de la muestra con SAFE/FIX apagados. Recuperación aproximada de 80 ms.

24 mediciones independientes con interpolación 32x/129 taps, a 44.1/48/96 kHz: senoide con picos entre muestras, tonos muy agudos (incluido 0.499 × sampleRate), impulsos, onda cuadrada, multitono y ruido. Ningún caso excedió −1 dBTP; máximo observado **-1.23627 dBTP**. Datos completos en true-peak.txt. SAFE emplea un margen interno conservador de 1.2 dB: puede actuar antes de llegar al techo mostrado. No se debe interpretar una prueba finita como garantía matemática sobre cualquier señal posible.

## TARGET y medición independiente

TARGET sustituye al selector K en la perilla: −36 a −6 dBFS RMS. La selección K se mueve al menú contextual del medidor y no afecta al DSP. Pruebas aprobadas para convergencia a −24/−18/−12 dBFS y para independencia entre escala K y nivel de audio. Migración de estado antiguo K-20 verificada a TARGET −20 dBFS. RIDE tiene recorte a 0.5× y subida a 1.5× el tiempo indicado.

## RANGE, SENSITIVITY y sidechain

Pruebas aprobadas para rango simétrico y rango cero, sensibilidad baja con tolerancia de ±3 dB, sensibilidad alta con TARGET exacto y migración de estados antiguos. La sensibilidad no modifica el tiempo RIDE.

Bus auxiliar mono/estéreo opcional probado: aprendizaje inicial, seguimiento de +4/−4 dB del instrumental, vuelta al TARGET ante silencio/desconexión, sin latencia extra y sin sobrescribir las muestras del bus auxiliar. El seguimiento queda limitado a ±6 dB alrededor del TARGET. El objetivo depende del instrumental cuando está conectado. No se ha comprobado el enrutado dentro de una sesión real de FL Studio.

## Entrada numérica 1.3.0

Doble clic en los valores bajo las perillas para editarlos. Enter aplica y Escape cancela. Se admiten coma decimal y unidades. Los valores se limitan al rango válido; texto no numérico y valores no finitos se descartan. TARGET acepta valores numéricos en dBFS. La escala K se elige desde el menú contextual del medidor.

## Escalado 1.3.0

Mismos límites que FET 67: 817 × 126 px hasta 4085 × 630 px, con proporción fija. Siempre abre al mínimo. Captura inicial en Preview-Min.png.

## Cambio visual 1.2.1

SAFE pasa a la izquierda de RIDE/OUT; FIX pasa a la derecha. SAFE incorpora LED morado lateral y aro morado en el interruptor como en la referencia. No se cambia el DSP ni los IDs de los botones.

## Interfaz y límites de la auditoría

Preview.png renderizada y revisada. Escalas situadas según la posición real de los parámetros. SAFE y FIX arrancan encendidos; LED morado lateral indica SAFE activo y aumenta de brillo al limitar. El medidor principal sigue siendo RMS K.

Las pruebas son offline con señales sintéticas y un host de validación; no se ha comprobado abriendo una sesión real en FL Studio. El nivel medio de 0 dB K está sujeto a límites, umbral y tiempos de respuesta; SAFE puede reducirlo para dar prioridad a los picos. FL Studio debe volver a escanear/reabrir el plugin para cargar el binario actualizado.
