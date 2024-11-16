<?php
// Setting header for CGI
header("Content-Type: text/html; charset=UTF-8");

// Fetch the city name from the URL (default to "Casablanca" if not provided)
$city = isset($_GET['city']) ? $_GET['city'] : 'Casablanca';
$country = 'Morocco';

// Prepare the URL for API request
$apiUrl = "https://api.aladhan.com/v1/timingsByCity?city=" . urlencode($city) . "&country=" . urlencode($country) . "&method=2";

// Fetch prayer times data from API
$response = file_get_contents($apiUrl);
$data = json_decode($response, true);

// Check if data was fetched successfully
if ($data['code'] == 200) {
    $prayerTimes = $data['data']['timings'];
} else {
    echo "<p>Error fetching prayer times. Please try again later.</p>";
    exit();
}
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Prayer Times for <?php echo htmlspecialchars($city); ?>, Morocco</title>
    
    <!-- Embed CSS directly in the PHP file -->
    <style>
        /* General Styles */
        body {
            font-family: Arial, sans-serif;
            background-color: #f7f7f7;
            color: #333;
            margin: 0;
            padding: 0;
        }

        .container {
            width: 80%;
            max-width: 800px;
            margin: 20px auto;
            padding: 20px;
            background-color: white;
            border-radius: 8px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
        }

        header {
            text-align: center;
            margin-bottom: 20px;
        }

        header h1 {
            font-size: 2rem;
            color: #4CAF50;
        }

        table {
            width: 100%;
            border-collapse: collapse;
            margin-top: 20px;
        }

        table th, table td {
            padding: 10px;
            text-align: center;
            border-bottom: 1px solid #ddd;
        }

        table th {
            background-color: #4CAF50;
            color: white;
        }

        table tr:hover {
            background-color: #f1f1f1;
        }

        footer {
            text-align: center;
            margin-top: 30px;
            font-size: 0.9rem;
        }

        footer a {
            color: #4CAF50;
            text-decoration: none;
        }

        footer a:hover {
            text-decoration: underline;
        }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>Prayer Times for <?php echo htmlspecialchars($city); ?>, Morocco</h1>
        </header>
        
        <table class="prayer-table">
            <thead>
                <tr>
                    <th>Prayer</th>
                    <th>Time</th>
                </tr>
            </thead>
            <tbody>
                <?php
                // Display the prayer times
                foreach ($prayerTimes as $prayer => $time) {
                    echo "<tr><td>" . ucfirst($prayer) . "</td><td>" . $time . "</td></tr>";
                }
                ?>
            </tbody>
        </table>

        <footer>
            <p>Data provided by <a href="https://aladhan.com" target="_blank">Aladhan API</a>.</p>
        </footer>
    </div>
</body>
</html>
