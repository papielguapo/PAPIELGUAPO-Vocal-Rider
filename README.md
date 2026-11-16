# PAPIELGUAPO - Vocal Rider 1.3.3 — Windows x64

![PAPIELGUAPO - Vocal Rider](Preview.png)

[Descargar VST3 y Standalone](https://github.com/papielguapo/PAPIELGUAPO-Vocal-Rider/releases/latest)

Nivelador vocal con TARGET, RANGE, RIDE, TOLERANCE y SILENCE. El objetivo de volumen es ajustable. OUT usa la misma referencia dBFS RMS que TARGET.

## Instalación y tamaño

Cierra el DAW y copia la carpeta completa `Binaries/PAPIELGUAPO - Vocal Rider.vst3` a `C:\Program Files\Common Files\VST3`. Reescanea y reabre el plugin. No copies solo el binario interior. La aplicación Standalone está en Binaries.

Abre siempre a 817 × 126 px. Se puede ampliar hasta 4085 × 630 px, con la misma proporción y límites que FET 67. Arrastra la esquina inferior derecha.

## Controles

| Control | Función |
|---|---|
| TARGET | Nivel medio vocal deseado, −36 a −6 dBFS RMS calibrado con senoide. Inicial: −14 dBFS. |
| RANGE | Corrección máxima simétrica, 0–24 dB. Inicial: ±6 dB. |
| RIDE | Suavizado, 50–1500 ms. El recorte usa 0.5× el valor y la subida 1.5×; el detector RMS añade su respuesta de 300 ms. Inicial: 300 ms. |
| TOLERANCE | Tolerancia alrededor de TARGET, de 0 a 3 dB. 0 dB busca el objetivo con precisión; 1.5 dB permite ±1.5 dB; 3 dB conserva variaciones de hasta ±3 dB. Inicial: 0 dB. |
| SILENCE | Umbral de detección, −80 a −20 dBFS. Evita perseguir señales demasiado bajas; no corta el audio. Inicial: −50 dBFS. |
| SAFE | A la izquierda de RIDE/OUT. Protección de picos, activada inicialmente. LED morado indica activación y brilla más al atenuar. |
| RIDE / OUT | Muestra corrección aplicada en dB o nivel medio de salida en dBFS RMS. |
| RIDER | A la derecha: activa la nivelación automática hacia TARGET. Apagado desactiva el rider, aunque SAFE puede seguir limitando. |
| POWER | Desactiva el procesamiento y conserva la latencia reportada al host. |

Doble clic en el número bajo una perilla para escribirlo. Enter aplica; Escape cancela. Admite coma decimal y unidades (dB, ms, dBFS). Se limitan los valores fuera de rango y se rechazan entradas inválidas/no finitas. Doble clic sobre el mando restaura su valor inicial.

## Medidores

OUT muestra el nivel medio de salida en dBFS RMS calibrado con senoide, después de SAFE. Su escala va de −48 a 0 dBFS; el valor numérico puede superar estos límites. TARGET utiliza la misma referencia. RIDE muestra la corrección del nivelador en dB: positivo sube, negativo baja; no incluye SAFE.

El detector RMS integra aproximadamente 300 ms, promedia la energía estéreo y aplica +3.0103 dB de calibración senoidal. Una senoide con pico −14 dBFS lee −14 dBFS. No es un medidor LUFS ni true peak. El LED SAFE brilla más cuando limita. La antigua selección K ya no aparece; su parámetro se conserva internamente para compatibilidad y no modifica el audio.

## Sidechain musical

Conecta el instrumental, sin la voz, al bus auxiliar **Sidechain** (mono o estéreo). No añade controles a la skin. Sin conexión, el objetivo es TARGET fijo. Con música audible aprende una referencia en unos 1.5 segundos y desplaza el objetivo hasta ±6 dB respecto a TARGET siguiendo los cambios sostenidos de nivel. RANGE sigue limitando la corrección y TOLERANCE su tolerancia.

Ante silencio/desconexión vuelve suavemente a TARGET. El instrumental solo se analiza; no se mezcla con la salida vocal. La referencia aprendida se reinicia al preparar/reiniciar el procesador o desconectar el bus y no se guarda en presets. No añade latencia.

En FL Studio, envía el instrumental a la pista de voz mediante **Sidechain to this track** y asigna la entrada auxiliar en **Processing → Connections** del wrapper. Mantén la voz como entrada principal. [Manual oficial](https://www.image-line.com/fl-studio-learning/fl-studio-online-manual/html/plugins/wrapper.htm).

## SAFE y límites

SAFE estima picos entre muestras con un FIR de 65 taps a 8x. Protección de −1 dBTP con un margen interno conservador de 1.2 dB (nivel interno −2.2 dBFS); puede actuar antes del techo. Latencia fija `ceil(0.003 × sampleRate) + 96` muestras, 240 muestras / 5 ms a 48 kHz, incluso con SAFE apagado. Se reporta al host para PDC. Recuperación aproximada: 80 ms. El audio se mantiene a la frecuencia de muestreo original.

TARGET es un objetivo medio, sujeto a RANGE, TOLERANCE, SILENCE, tiempos de respuesta y SAFE. Si SAFE limita, los picos tienen prioridad. El medidor principal es RMS, no true peak. El plugin no clasifica voz frente a respiraciones/ruido y no escribe curvas en la automatización del DAW.

## Cambios 1.3.3

FIX pasa a llamarse RIDER. El botón conserva su ID interno `fix` y la automatización existente. SENSITIVITY se sustituye por TOLERANCE expresado directamente en dB: `tolerancia = 3 × (1 − sensibilidad/100)`. Por ejemplo, el antiguo 25% se convierte a 2.25 dB; el 100%, a 0 dB. La función de audio es la misma para valores equivalentes. Los estados se convierten automáticamente. Revisa los clips de automatización de SENSITIVITY: ahora se automatiza el parámetro `tolerance` en dB y su sentido es inverso al porcentaje anterior.

## Presets anteriores

Al cargar un estado sin TARGET, se transforma la antigua referencia K en un objetivo absoluto: K-12 → −12, K-14 → −14, K-20 → −20 dBFS. Los antiguos límites boost/cut se convierten en RANGE usando el menor y TOLERANCE 0 dB. Revisa automatizaciones antiguas: K se conserva por compatibilidad y ya no modifica la lectura ni el audio; si quieres mover el nivel, automatiza TARGET.

## Compilación y validación

C++17, CMake ≥3.22, Visual Studio y JUCE. Configura `Rider_JUCE_PATH` si no está en C:/JUCE. Compila Release x64; `Rider_BUILD_TESTS=ON` habilita pruebas de audio y edición numérica. La instalación automática al compilar está desactivada. Informes en Validation.

JUCE usa AGPLv3 o licencia comercial. Según el uso y distribución puede ser necesaria una licencia comercial de JUCE. Se incluye JUCE-LICENSE.md.

## Distribución 1.3.3

Orden de izquierda a derecha: TARGET → RANGE → RIDE → TOLERANCE → SILENCE. Cada control conserva sus ajustes, unidades, escalas, entrada numérica y automatización.

## Cambios 1.3.3

Nombre de archivo y producto: PAPIELGUAPO - Vocal Rider. Skin: VOCAL RIDER. OUT en dBFS RMS, de −48 a 0; RIDE conserva dB de corrección. Identidad VST3 y parámetros existentes conservados.
