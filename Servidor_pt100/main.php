<?php
session_start();
if (!isset($_SESSION['usuario'])) {
    header("Location: login.php");
    exit;
}
?>
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<title>UIDET IEA - Panel Principal</title>
<style>
body {
    font-family: Arial, sans-serif;
    padding: 20px;
    background-color: #fafafa;
}
.header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 25px;
}
.header h2 {
    margin: 0;
}
.header img {
    height: 60px;
}
.tabs {
    display: flex;
    margin-bottom: 20px;
    flex-wrap: wrap;
}
.tab {
    padding: 10px 15px;
    background: #ddd;
    margin-right: 10px;
    margin-bottom: 5px;
    cursor: pointer;
    border-radius: 5px;
    transition: background 0.3s;
}
.tab:hover {
    background: #bbb;
}
.content {
    display: none;
    padding: 15px;
    border: 1px solid #ccc;
    border-radius: 5px;
    background-color: #fff;
}
input, button {
    margin: 5px 0;
    padding: 5px 10px;
}
.data-table {
    border-collapse: collapse;
    width: 100%;
    margin-top: 10px;
}
.data-table th, .data-table td {
    border: 1px solid #ccc;
    padding: 8px;
    text-align: center;
}
</style>
<script>
function showContent(id) {
    const contents = document.querySelectorAll('.content');
    contents.forEach(c => c.style.display = 'none');
    document.getElementById(id).style.display = 'block';
}
</script>
</head>
<body>

<div class="header">
    <h2>UIDET IEA</h2>
    <img src="img/logo_uidet.png" alt="Logo UIDET">
</div>

<a href="logout.php">Cerrar sesión</a>

<div class="tabs">
    <div class="tab" onclick="showContent('aguas')">Aguas</div>
    <div class="tab" onclick="showContent('electronica')">Electrónica</div>
    <div class="tab" onclick="showContent('rtt')">RTT</div>
    <div class="tab" onclick="showContent('taller')">Taller</div>
</div>

<!-- Sección AGUAS -->
<div class="content" id="aguas">
    <h3>Aguas</h3>
    <p>Contenido del módulo de Aguas.</p>
</div>

<!-- Sección ELECTRÓNICA -->
<div class="content" id="electronica">
    <h3>Electrónica</h3>
    <p>Registrar datos de sensores (presión, temperatura, etc.)</p>

    <form method="post" action="save_data.php">
        <label>Presión (bar):</label><br>
        <input type="number" step="0.01" name="presion" required><br>
        <label>Temperatura (°C):</label><br>
        <input type="number" step="0.1" name="temperatura" required><br>
        <button type="submit">Guardar datos</button>
    </form>

    <h4>Datos registrados:</h4>
    <table class="data-table">
        <tr><th>Fecha</th><th>Presión (bar)</th><th>Temperatura (°C)</th></tr>
        <?php
        $archivo = 'data/electronica.csv';
        if (file_exists($archivo)) {
            $rows = array_reverse(file($archivo)); // mostrar últimos primero
            foreach ($rows as $linea) {
                $campos = str_getcsv(trim($linea));
                echo "<tr>";
                foreach ($campos as $dato) echo "<td>" . htmlspecialchars($dato) . "</td>";
                echo "</tr>";
            }
        } else {
            echo "<tr><td colspan='3'>Sin datos</td></tr>";
        }
        ?>
    </table>
</div>

<!-- Sección RTT -->
<div class="content" id="rtt">
    <h3>RTT</h3>
    <p>Contenido del módulo RTT.</p>
</div>

<!-- Sección TALLER -->
<div class="content" id="taller">
    <h3>Taller</h3>
    <p>Contenido del Taller.</p>
</div>

<script>
// Mostrar el primer tab al cargar
showContent('aguas');
</script>

</body>
</html>
