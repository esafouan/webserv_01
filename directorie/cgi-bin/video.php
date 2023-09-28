
<?php
    $contentTypeToExtension = [
        'image/jpeg' => '.jpg',
        'image/png' => '.png',
        'image/gif' => '.gif',
        'video/mp4' => '.mp4',
        'video/quicktime' => '.mov',
        'application/pdf' => '.pdf',
        'application/msword' => '.doc',
        'application/vnd.openxmlformats-officedocument.wordprocessingml.document' => '.docx',
        'application/vnd.ms-excel' => '.xls',
        'application/vnd.openxmlformats-officedocument.spreadsheetml.sheet' => '.xlsx',
        'application/zip' => '.zip',
    ];

    $contentType = isset($_SERVER['CONTENT_TYPE']) ? $_SERVER['CONTENT_TYPE'] : '';
    $fileExtension = isset($contentTypeToExtension[$contentType]) ? $contentTypeToExtension[$contentType] : '';

    $inputData = file_get_contents("php://input");
    $filename = "../upload/" . bin2hex(random_bytes(8)) . $fileExtension;

    file_put_contents($filename, $inputData);
    if (file_exists($filename)) {
        echo "Data written to $filename successfully.";
    } else {
        echo "Failed to write data to $filename.";
    }

?>


