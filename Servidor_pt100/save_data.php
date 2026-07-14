<?php
// ===============================
// save_data.php - UIDET IEA Server
// ===============================

if ($_SERVER['REQUEST_METHOD'] === 'POST') {

    // Leer variables enviadas por ESP
    $presion = $_POST['presion'] ?? '';
    $temperatura = $_POST['temperatura'] ?? '';

    // Si al menos una variable tiene valor
    if ($presion !== '' || $temperatura !== '') {

        // Generar timestamp y definir ruta absoluta
        $fecha = date('Y-m-d H:i:s');
        $archivo = '/var/www/html/data/electronica.csv'; // ✅ ruta absoluta
        $linea = "$fecha,$presion,$temperatura\n";

        // Intentar guardar el archivo
        if (file_put_contents($archivo, $linea, FILE_APPEND | LOCK_EX) !== false) {
            http_response_code(200);
            echo "OK";
        } else {
            http_response_code(500);
            echo "ERROR: no se pudo escribir el archivo";
        }

    } else {
        http_response_code(400);
        echo "ERROR: faltan datos";
    }

} else {
    http_response_code(405);
    echo "ERROR: método no permitido";
}
?>
