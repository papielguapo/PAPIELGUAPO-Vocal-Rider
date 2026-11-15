# Vocal Rider K 1.0.0 — Windows x64

![Interfaz de Vocal Rider K](Preview.png)

[Descargar VST3 y Standalone para Windows x64](https://github.com/papielguapo/Vocal-Rider-K/releases/latest)

Vocal rider independiente con la estética de VCA SSL: aluminio cepillado, mandos morados, interruptores y medidor LED. Automatiza la ganancia de una pista de voz mediante un detector RMS de 300 ms y ganancia enlazada entre canales. Sin saturación añadida ni latencia de anticipación.

## Instalación

Cierra el DAW. Copia la carpeta completa `Binaries/Vocal Rider K.vst3` a `C:\Program Files\Common Files\VST3\` y vuelve a escanear plugins. No copies únicamente el archivo interior. No reemplaza VCA SSL. `Vocal Rider K.exe` permite usarlo sin DAW; selecciona tu dispositivo en sus ajustes de audio.

## Uso

1. Inserta el plugin en una pista vocal mono o estéreo, antes de compresores y efectos.
2. Selecciona K-12, K-14 o K-20. El objetivo es siempre 0 dB K automático. El selector de tres posiciones está inmediatamente a la izquierda del medidor.
3. Ajusta MAX BOOST y MAX CUT para limitar cuánto puede mover la voz. Valores iniciales: +6 / −12 dB.
4. Ajusta SILENCE por encima del ruido de fondo y por debajo de la voz más suave. En silencio el rider vuelve lentamente a 0 dB, sin cortar el audio.
5. RIDE controla el tiempo de suavizado de ganancia: 50–1500 ms. El detector RMS añade su propia respuesta de 300 ms.
6. METER alterna RIDE (ganancia aplicada, positiva o negativa) y OUT (RMS relativo a la referencia K). FIX, a la izquierda del botón RIDE/OUT, activa la nivelación automática a 0 dB K. Apagado deja la señal a ganancia unidad. FIX arranca encendido.

No hay ajuste TARGET ni TRIM de salida: el objetivo queda fijado a 0 dB K. K-12 / K-14 / K-20 usan respectivamente −12 / −14 / −20 dBFS como referencia senoidal. Una senoide de pico −14 dBFS lee 0 K en K-14. Se usa RMS calibrado con +3.0103 dB, no LUFS ni ponderación K de BS.1770. El sistema K original también incluye calibración de monitores; el plugin no calibra tus altavoces.

Referencia técnica: https://www.digido.com/portfolio-item/level-practices-part-2/

La medición estéreo promedia la energía de ambos canales; aplica la misma ganancia a ambos para conservar su balance. FIX, POWER y el bypass del host hacen una transición de 10 ms hacia señal seca. Doble clic devuelve cada mando a su valor inicial. Los parámetros se pueden automatizar y se guardan en la sesión.

## Límites

El objetivo se alcanza si la señal supera SILENCE y los límites de ganancia lo permiten. No es un limitador de picos ni un medidor true peak: deja margen y usa un limitador posterior si lo necesitas. No utiliza una entrada de música para adaptar la voz al acompañamiento. No escribe curvas en la pista de automatización del DAW; calcula la ganancia internamente.

## Compilación

C++17, CMake >=3.22, Visual Studio y JUCE 7/8/9. Configura `Rider_JUCE_PATH` si JUCE no está en C:/JUCE. Compila en Release x64. La opción `Rider_BUILD_TESTS=ON` habilita pruebas DSP. `Rider_COPY_PLUGIN_AFTER_BUILD` está desactivada para entregar el plugin sin alterar instalaciones.

JUCE se distribuye bajo AGPLv3 o licencia comercial. Puede ser necesaria una licencia comercial de JUCE según el uso y distribución que hagas. Se incluye su archivo de licencia.
