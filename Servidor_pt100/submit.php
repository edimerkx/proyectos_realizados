<?php
session_start();

// Inicializa un mensaje de error vacío
$error = '';

if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    // Obtiene los datos del formulario
    $username = $_POST['username'];
    $password = $_POST['password'];

    // Comparar credenciales (puedes mejorar esto con hashing)
    if ($username === 'admin' && $password === '1234') {
        // Iniciar sesión y redirigir a la página principal
        $_SESSION['loggedin'] = true;
        header("Location: main.html");
        exit();
    } else {
        $error = "Nombre de usuario o contraseña incorrectos.";
    }
}
?>

<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <title>Iniciar Sesión</title>
    <style>
        /* Estilos previos... */
    </style>
</head>
<body>

<form action="" method="post">
    <h2>Iniciar Sesión</h2>
    <input type="text" name="username" placeholder="Nombre de Usuario" required>
    <input type="password" name="password" placeholder="Contraseña" required>
    <input type="submit" value="Iniciar Sesión">
</form>

<?php if ($error): ?>
    <p style="color:red;"><?php echo htmlspecialchars($error); ?></p>
<?php endif; ?>

</body>
</html>

