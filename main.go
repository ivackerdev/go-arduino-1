package main

import (
	"log"
	"net/http"
	"sync"
	"time"
)

// Pixel de seguimiento en formato GIF: GIF89a, 1x1 pixel, sin compresión, transparente.
var trackingPixel = []byte{
	0x47, 0x49, 0x46, 0x38, 0x39, 0x61, // cabecera GIF
	0x01, 0x00, 0x01, 0x00, // anchura y altura de 1x1
	0x80,             // flags: colormap local, sin interlace, sin tabla de colores de clasificación, sin tabla de colores global
	0x00,             // tamaño de la tabla de colores - no se usa
	0x00, 0x00, 0x00, // color de fondo: negro
	0x21, 0xF9, 0x04, // bloque de extensión de gráficos: indica transparencia
	0x01, 0x00, 0x00, 0x00, // sub-bloque de extensión de gráficos
	0x2C,                   // introducción de imagen
	0x00, 0x00, 0x00, 0x00, // posición y tamaño de la imagen
	0x01, 0x00, 0x01, 0x00, // más tamaño de la imagen
	0x00,       // banderas
	0x02,       // tamaño mínimo del bloque de datos LZW
	0x02,       // tamaño del bloque de datos
	0x4C, 0x01, // datos de imagen
	0x00, // bloque terminator
}

var (
	count int // Contador de llamadas al endpoint del pixel
	mutex sync.Mutex
)

func trackPixelHandler(w http.ResponseWriter, r *http.Request) {
	mutex.Lock()
	count++ // Incrementa el contador de llamadas
	mutex.Unlock()

	// Obtiene la dirección IP del cliente
	ip := r.RemoteAddr
	// Obtiene la fecha y hora actual
	timestamp := time.Now().Format("2006-01-02 15:04:05")

	// Registra la información en la consola
	log.Printf("Request #%d from %s at %s\n", count, ip, timestamp)

	// Establece las cabeceras para indicar al navegador que se trata de una imagen GIF y evitar su almacenamiento en caché
	w.Header().Set("Content-Type", "image/gif")
	w.Header().Set("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0")
	w.Header().Set("Pragma", "no-cache")
	w.Header().Set("Expires", "Fri, 01 Jan 1990 00:00:00 GMT")

	// Escribe el pixel GIF al cuerpo de la respuesta
	w.Write(trackingPixel)
}

func main() {
	// Define la ruta y el manejador
	http.HandleFunc("/p.gif", trackPixelHandler)

	// Inicia el servidor en el puerto 8080
	log.Println("Server started on port 80")
	if err := http.ListenAndServe(":80", nil); err != nil {
		log.Fatal("ListenAndServe: ", err)
	}
}
