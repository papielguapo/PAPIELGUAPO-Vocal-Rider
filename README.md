# Vocal Rider K 1.2.0 — Windows x64

![Interfaz de Vocal Rider K](Preview.png)

[Descargar VST3 y Standalone para Windows x64](https://github.com/papielguapo/Vocal-Rider-K/releases/latest)

Vocal rider independiente con la estética de VCA SSL: aluminio cepillado, mandos morados, interruptores y medidor LED. Automatiza la ganancia de una pista de voz mediante un detector RMS de 300 ms y ganancia enlazada entre canales. Sin saturación añadida. SAFE incorpora anticipación y reporta su latencia al DAW.

## Instalación

Cierra el DAW. Copia la carpeta completa `Binaries/Vocal Rider K.vst3` a `C:\Program Files\Common Files\VST3\` y vuelve a escanear plugins. No copies únicamente el archivo interior. No reemplaza VCA SSL. `Vocal Rider K.exe` permite usarlo sin DAW; selecciona tu dispositivo en sus ajustes de audio.

## Novedades 1.2.0

RANGE sustituye a los límites independientes de boost/cut. SENSITIVITY controla la tolerancia alrededor del objetivo. Sidechain musical mono/estéreo opcional, gestionado desde el host, sin controles adicionales en la skin. Se conservan SAFE, FIX y las escalas impresas.

SAFE estima picos entre muestras mediante un interpolador FIR a 8x, anticipa la reducción y aplica la misma ganancia a ambos canales. El audio conserva su frecuencia de muestreo original. La latencia fija es `ceil(0.003 × sampleRate) + 96` muestras: 240 muestras / 5 ms a 48 kHz, incluso con SAFE, POWER o el bypass apagados. FL Studio puede compensarla mediante PDC. La transición de SAFE dura 10 ms y su recuperación es de unos 80 ms.

## Uso

1. Inserta el plugin en una pista vocal mono o estéreo, antes de compresores y efectos.
2. Selecciona K-12, K-14 o K-20. Sin sidechain, el objetivo es 0 dB K automático. El selector de tres posiciones está inmediatamente a la izquierda del medidor.
3. Ajusta RANGE (0–24 dB) para limitar la subida y bajada por igual. RANGE 6 dB permite de −6 a +6 dB. SENSITIVITY 100% busca exactamente el cero K; 0% permite una tolerancia de ±3 dB. Los valores intermedios interpolan esa tolerancia: 50% permite ±1.5 dB. El valor inicial es 100%.
4. Ajusta SILENCE por encima del ruido de fondo y por debajo de la voz más suave. En silencio el rider vuelve lentamente a 0 dB, sin cortar el audio.
5. RIDE controla el tiempo de suavizado de ganancia: 50–1500 ms. El detector RMS añade su propia respuesta de 300 ms.
6. METER alterna RIDE (ganancia aplicada, positiva o negativa) y OUT (RMS relativo a la referencia K). FIX, a la izquierda del botón RIDE/OUT, activa la nivelación automática a 0 dB K. Apagado desactiva la ganancia del rider; SAFE puede seguir limitando los picos si está encendido. FIX arranca encendido. SAFE, a la derecha de RIDE/OUT, activa la protección de picos; también arranca encendido. Su LED ámbar indica reducción de picos.

No hay ajuste TARGET ni TRIM de salida: sin sidechain, la referencia central queda en 0 dB K. K-12 / K-14 / K-20 usan respectivamente −12 / −14 / −20 dBFS como referencia senoidal. Una senoide de pico −14 dBFS lee 0 K en K-14. Se usa RMS calibrado con +3.0103 dB, no LUFS ni ponderación K de BS.1770. El sistema K original también incluye calibración de monitores; el plugin no calibra tus altavoces.

Referencia técnica: https://www.digido.com/portfolio-item/level-practices-part-2/

La medición estéreo promedia la energía de ambos canales; aplica la misma ganancia a ambos para conservar su balance. FIX, SAFE, POWER y el bypass del host hacen una transición de 10 ms hacia señal seca. Doble clic devuelve cada mando a su valor inicial. Los parámetros se pueden automatizar y se guardan en la sesión.

## Sidechain musical sin controles nuevos

El VST3 expone una entrada auxiliar opcional llamada **Sidechain**. Conecta el instrumental sin la voz. El plugin mide su nivel y aprende una referencia durante aproximadamente 1.5 segundos de señal audible; después adapta el objetivo de la voz a sus cambios, con un límite de ±6 dB alrededor del cero K. RANGE sigue limitando la ganancia total y SENSITIVITY aplica su tolerancia al objetivo que corresponda.

Sin sidechain, o si queda en silencio, vuelve suavemente al cero K fijo. El instrumental solo se analiza: no se mezcla en la salida vocal. El aprendizaje se reinicia al preparar/reiniciar el procesador o desconectar el bus. No se guarda una referencia musical aprendida en el preset. La latencia no cambia con el sidechain.

En FL Studio: envía la pista/bus instrumental a la pista de voz mediante **Sidechain to this track**; en el wrapper del VST3, abre **Processing → Connections** y asigna esa entrada al bus Sidechain. Mantén la voz como entrada principal. Comprueba el enrutado antes de reproducir. [Manual oficial de Image-Line](https://www.image-line.com/fl-studio-learning/fl-studio-online-manual/html/plugins/wrapper.htm).

## Compatibilidad de ajustes

Los estados antiguos con MAX BOOST / MAX CUT se convierten a RANGE tomando el menor de ambos límites; SENSITIVITY pasa a 100%. Una configuración asimétrica anterior no puede conservarse exactamente con un solo rango simétrico. Revisa los clips de automatización antiguos de esos dos parámetros: ahora sus IDs y funciones son RANGE y SENSITIVITY. El bus sidechain arranca desactivado; debes conectarlo desde el host.

## Límites

El objetivo se alcanza si la señal supera SILENCE y los límites de ganancia lo permiten. SAFE añade un limitador de protección con estimación true peak a 8x y techo de protección de −1 dBTP. Usa un margen interno conservador de 1.2 dB (nivel interno −2.2 dBFS), por lo que puede actuar antes de llegar al techo. El LED indica cuándo atenúa. Con SAFE apagado no hay protección de picos. El medidor principal sigue siendo RMS K, no un medidor true peak. Si SAFE actúa, la protección de picos tiene prioridad sobre el objetivo medio de 0 dB K. Al conectar el sidechain, el objetivo sigue las variaciones de nivel del instrumental dentro de ±6 dB alrededor del cero K. No escribe curvas en la pista de automatización del DAW; calcula la ganancia internamente.

## Compilación

C++17, CMake >=3.22, Visual Studio y JUCE 7/8/9. Configura `Rider_JUCE_PATH` si JUCE no está en C:/JUCE. Compila en Release x64. La opción `Rider_BUILD_TESTS=ON` habilita pruebas DSP. `Rider_COPY_PLUGIN_AFTER_BUILD` está desactivada para entregar el plugin sin alterar instalaciones.

JUCE se distribuye bajo AGPLv3 o licencia comercial. Puede ser necesaria una licencia comercial de JUCE según el uso y distribución que hagas. Se incluye su archivo de licencia.
