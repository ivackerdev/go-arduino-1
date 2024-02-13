# README para el Proyecto de Arduino ESP8266 y Servidor Backend en Go

Este proyecto integra un módulo ESP8266 que ejecuta un servidor web para la gestión de archivos en SPIFFS y se conecta a una red WiFi, con un servidor backend en Go que sirve un píxel de seguimiento. El servidor web del ESP8266 realiza peticiones HTTP para descargar imágenes y almacenarlas, mientras que el servidor en Go registra estas peticiones y sirve un píxel de seguimiento en formato GIF.

## Características del Proyecto de Arduino

- **Conexión WiFi**: Configura la conexión a una red WiFi especificada.
- **Servidor Web**: Inicia un servidor web en el puerto 80 que permite gestionar archivos en SPIFFS.
- **Gestión de Archivos y Almacenamiento**: Permite listar, eliminar archivos y ver información de almacenamiento en SPIFFS.
- **Descarga Automática de Imágenes**: Descarga imágenes desde una dirección específica y las almacena en SPIFFS.
- **Control de Ejecución**: Permite detener el loop principal a través de una petición web.

## Servidor Backend en Go

El servidor backend está escrito en Go y sirve un píxel de seguimiento en formato GIF. Este píxel es de 1x1 pixel, transparente, y está diseñado para ser utilizado en operaciones de seguimiento web sin afectar la experiencia del usuario.

### Características del Servidor Backend

- **Píxel de Seguimiento GIF**: Sirve un píxel de seguimiento transparente de 1x1.
- **Registro de Peticiones**: Cuenta y registra cada petición al endpoint del píxel, incluyendo la dirección IP del cliente y la marca de tiempo.
- **Evitar Caché**: Establece cabeceras HTTP para evitar el almacenamiento en caché del píxel por parte de los navegadores.

### Cómo Funciona

El servidor define un handler para el endpoint `/p.gif`, que incrementa un contador de peticiones, registra la petición y responde con el píxel de seguimiento. Utiliza mutex para asegurar que el contador se incremente de manera segura en un entorno concurrente.

### Instrucciones de Uso

Para ejecutar el servidor backend en Go:

1. Asegúrate de tener Go instalado en tu sistema.
2. Copia el código del servidor en un archivo `.go`.
3. Ejecuta el servidor con `go run <archivo>.go`.
4. El servidor se iniciará en el puerto 80, listo para servir el píxel de seguimiento a las peticiones HTTP.

## Integración

El módulo ESP8266 realiza peticiones HTTP al servidor backend en Go para descargar el píxel de seguimiento, lo que permite registrar la actividad del dispositivo en el servidor.

## Contribuciones

Este es un proyecto abierto a contribuciones. Se invita a los usuarios a colaborar en la mejora del código, documentación, y reporte de errores.

## Licencia

Este proyecto se distribuye bajo la licencia MIT, que permite su uso, copia, modificación, distribución, y venta tanto en proyectos privados como comerciales.

Este README combina información sobre dos componentes clave del proyecto: el módulo ESP8266 que ejecuta un servidor web y gestiona archivos en SPIFFS, y el servidor backend en Go que sirve un píxel de seguimiento. Ambos componentes trabajan juntos para facilitar la gestión de archivos, la conexión a internet y el seguimiento de peticiones HTTP en proyectos de IoT.
![ss1](https://github.com/ivackerdev/go-arduino-1/assets/65930496/fb5b8a5d-4ae8-4d8d-8571-0b668afddd7e)
![ss2](https://github.com/ivackerdev/go-arduino-1/assets/65930496/78cd67ee-e293-4517-a019-5f9d55fa4f6e)
