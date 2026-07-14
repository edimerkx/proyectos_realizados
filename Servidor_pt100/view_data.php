<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Datos de Sensores</title>
</head>
<body>
    <h2>Datos de Temperatura y Humedad</h2>
    <table border="1">
        <tr>
            <th>Timestamp</th>
            <th>Temperature (°C)</th>
            <th>Humidity (%)</th>
        </tr>
        <?php
        if (file_exists('data.csv')) {
            // Leer el archivo CSV
            if (($handle = fopen('data.csv', 'r')) !== FALSE) {
                // Saltar el encabezado
                fgetcsv($handle);
                while (($data = fgetcsv($handle)) !== FALSE) {
                    echo '<tr>';
                    echo '<td>' . htmlspecialchars($data[0]) . '</td>';
                    echo '<td>' . htmlspecialchars($data[1]) . '</td>';
                    echo '<td>' . htmlspecialchars($data[2]) . '</td>';
                    echo '</tr>';
                }
                fclose($handle);
            }
        }
        ?>
    </table>
</body>
</html>
