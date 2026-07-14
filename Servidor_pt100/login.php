<?php
session_start();

// Si ya hay sesión iniciada, ir directo al main
if (isset($_SESSION['usuario'])) {
    header("Location: main.php");
    exit;
}

// Procesar el formulario
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $usuario_valido = 'admin';
    $clave_valida = '1234';

    $usuario = $_POST['username'] ?? '';
    $clave = $_POST['password'] ?? '';

    if ($usuario === $usuario_valido && $clave === $clave_valida) {
        $_SESSION['usuario'] = $usuario;
        header("Location: main.php");
        exit;
    } else {
        $error = "Usuario o contraseña incorrectos.";
    }
}
?>
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<title>Iniciar Sesión</title>
<style>
body {
    font-family: Arial, sans-serif;
    display: flex;
    justify-content: center;
    align-items: center;
    height: 100vh;
    background-color: #f0f0f0;
}
form {
    background: white;
    padding: 20px;
    border-radius: 5px;
    box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
    width: 300px;
}
input[type="text"], input[type="password"] {
    width: 100%;
    padding: 10px;
    margin: 5px 0 10px;
    border: 1px solid #ccc;
    border-radius: 4px;
}
input[type="submit"] {
    background-color: #4CAF50;
    color: white;
    border: none;
    padding: 10px;
    cursor: pointer;
    border-radius: 4px;
    width: 100%;
}
.error {
    color: red;
    font-size: 14px;
    text-align: center;
}
</style>
</head>
<body>

<form method="post" action="">
    <h2>UIDET IEA</h2>
    <input type="text" name="username" placeholder="Usuario" required>
    <input type="password" name="password" placeholder="Contraseña" required>
    <input type="submit" value="Ingresar">
    <?php if (isset($error)): ?>
        <p class="error"><?php echo $error; ?></p>
    <?php endif; ?>
</form>

</body>
</html>
