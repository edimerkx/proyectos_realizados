<?php
if (isset($_GET['temperature']) && isset($_GET['humidity'])) {
    $temperature = $_GET['temperature'];
    $humidity = $_GET['humidity'];
    $timestamp = date("Y-m-d H:i:s"); // Obtener la fecha y hora actual

    // Crear o abrir el archivo CSV en modo de adición
    $file = fopen('data.csv', 'a');

    // Escribir los encabezados si el archivo está vacío
    if (filesize('data.csv') == 0) {
        fputcsv($file, ['timestamp', 'temperature', 'humidity']);
    }

    // Guardar los datos en el archivo CSV
    fputcsv($file, [$timestamp, $temperature, $humidity]);

    fclose($file); // Cerrar el archivo

    echo "Datos recibidos";
} else {
    echo "Datos no válidos";
}
?>

