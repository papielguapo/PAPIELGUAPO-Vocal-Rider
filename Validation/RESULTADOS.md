# PAPIELGUAPO - Vocal Rider 1.3.3 — Auditoría

Versión Windows x64 con RANGE simétrico, TOLERANCE y sidechain musical opcional sin controles nuevos en la skin. Sin sidechain, RIDER busca TARGET ajustable. OUT mide dBFS RMS; RIDE mide dB de corrección. Se conservan SAFE y las escalas impresas; no hay TRIM.

## Compilación e instalación

Release MSVC x64: VST3 y Standalone compilados correctamente. Instalado en C:/Program Files/Common Files/VST3/PAPIELGUAPO - Vocal Rider.vst3. SHA-256 del binario instalado idéntico al del paquete: `544d759b5784bf4032ed0a93ac36afcc80863bb74fdeb5e2e525828ac3ae9762`.

## Pluginval

pluginval 1.0.4, nivel de exigencia 10, semilla 121453, ejecutado sobre la copia instalada en Common Files/VST3 de la versión 1.3.3: **SUCCESS, salida 0**. Sin excluir pruebas de GUI. Tasas 44100/48000/96000 Hz y bloques 64/128/256/512/1024. Incluye automatización y fuzzing de parámetros. Log íntegro en pluginval-level10.txt.

## Audio

Pruebas DSP aprobadas: convergencia a distintos niveles TARGET e independencia de la escala K, límites de boost/cut, silencio, bypass, RIDER, mono/estéreo enlazado, recuperación de estado y presencia de TARGET y ausencia de TRIM.

SAFE: detector FIR 8x de 65 taps, ganancia enlazada, anticipación de 3 ms más 96 muestras de margen y detector. Latencia fija de 240 muestras / 5 ms a 48 kHz, reportada al host e idéntica con SAFE apagado. La prueba de impulso verifica la latencia y la conservación exacta de la muestra con SAFE/RIDER apagados. Recuperación aproximada de 80 ms.

24 mediciones independientes con interpolación 32x/129 taps, a 44.1/48/96 kHz: senoide con picos entre muestras, tonos muy agudos (incluido 0.499 × sampleRate), impulsos, onda cuadrada, multitono y ruido. Ningún caso excedió −1 dBTP; máximo observado **-1.23627 dBTP**. Datos completos en true-peak.txt. SAFE emplea un margen interno conservador de 1.2 dB: puede actuar antes de llegar al techo mostrado. No se debe interpretar una prueba finita como garantía matemática sobre cualquier señal posible.

## RIDER y TOLERANCE

RIDER conserva el ID interno fix. TOLERANCE se expresa en dB (0–3) y sustituye al porcentaje de SENSITIVITY. Migración verificada: 25% antiguo equivale a 2.25 dB. Los presets conservan su comportamiento de audio para valores equivalentes; los clips antiguos de automatización de sensibilidad requieren revisión y conversión al nuevo parámetro tolerance.

## TARGET y medición independiente

TARGET sustituye al selector K en la perilla: −36 a −6 dBFS RMS. OUT muestra dBFS RMS absolutos, con escala de −48 a 0 y sin menú K. El parámetro K antiguo se conserva por compatibilidad y no afecta al DSP. Pruebas aprobadas para convergencia a −24/−18/−12 dBFS y para independencia entre escala K y nivel de audio. Migración de estado antiguo K-20 verificada a TARGET −20 dBFS. RIDE tiene recorte a 0.5× y subida a 1.5× el tiempo indicado.

## RANGE, TOLERANCE y sidechain

Pruebas aprobadas para rango simétrico y rango cero, tolerancia de 3 dB que conserva variaciones y tolerancia de 0 dB con TARGET exacto y migración de estados antiguos. La tolerancia no modifica el tiempo RIDE.

Bus auxiliar mono/estéreo opcional probado: aprendizaje inicial, seguimiento de +4/−4 dB del instrumental, vuelta al TARGET ante silencio/desconexión, sin latencia extra y sin sobrescribir las muestras del bus auxiliar. El seguimiento queda limitado a ±6 dB alrededor del TARGET. El objetivo depende del instrumental cuando está conectado. No se ha comprobado el enrutado dentro de una sesión real de FL Studio.

## Entrada numérica 1.3.3

Doble clic en los valores bajo las perillas para editarlos. Enter aplica y Escape cancela. Se admiten coma decimal y unidades. Los valores se limitan al rango válido; texto no numérico y valores no finitos se descartan. TARGET acepta valores numéricos en dBFS. La escala K se elige desde el menú contextual del medidor.

## Escalado 1.3.3

Mismos límites que FET 67: 817 × 126 px hasta 4085 × 630 px, con proporción fija. Siempre abre al mínimo. Captura inicial en Preview-Min.png.

## Cambio visual 1.2.1

SAFE pasa a la izquierda de RIDE/OUT; RIDER pasa a la derecha. SAFE incorpora LED morado lateral y aro morado en el interruptor como en la referencia. No se cambia el DSP ni los IDs de los botones.

## Interfaz y límites de la auditoría

Preview.png renderizada y revisada. Orden visual: TARGET → RANGE → RIDE → TOLERANCE → SILENCE. Escalas situadas según la posición real de los parámetros. SAFE y RIDER arrancan encendidos; LED morado lateral indica SAFE activo y aumenta de brillo al limitar. El medidor OUT muestra dBFS RMS de −48 a 0; RIDE conserva dB de corrección.

Las pruebas son offline con señales sintéticas y un host de validación; no se ha comprobado abriendo una sesión real en FL Studio. El nivel medio de 0 dB K está sujeto a límites, umbral y tiempos de respuesta; SAFE puede reducirlo para dar prioridad a los picos. FL Studio debe volver a escanear/reabrir el plugin para cargar el binario actualizado.
