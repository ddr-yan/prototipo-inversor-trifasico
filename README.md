# prototipo-inversor-trifasico
<html>
<body>
<!--StartFragment--><h2 data-start="415" data-end="467">Prototipo de inversor trifásico para motor BLDC</h2>
<p data-start="468" data-end="750">Este repositorio contiene el diseño, modelado y validación experimental de un inversor trifásico de seis pasos para el accionamiento de un motor BLDC en lazo abierto.<br data-start="634" data-end="637">
El proyecto combina análisis teórico, selección de componentes, simulaciones y construcción física del prototipo.</p>
<h2 data-start="752" data-end="776">Descripción general</h2>
<p data-start="777" data-end="1269">El trabajo desarrolla un <strong data-start="802" data-end="867">modelo matemático de la dinámica de conmutación de los MOSFET</strong>, utilizado para estimar los tiempos de encendido y apagado en las ramas del inversor.<br data-start="953" data-end="956">
Posteriormente se implementa un <strong data-start="988" data-end="1027">prototipo con componentes discretos</strong> y se comparan los tiempos calculados con mediciones obtenidas mediante osciloscopio.<br data-start="1112" data-end="1115">
Las diferencias entre ambos resultados se mantienen dentro del mismo orden de magnitud, lo que confirma la validez del modelo analítico en esta topología.</p>
<h2 data-start="1271" data-end="1301">Contenido del repositorio</h2>
<ul data-start="1302" data-end="1718">
<li data-start="1302" data-end="1376">
<p data-start="1304" data-end="1376"><strong data-start="1304" data-end="1314">/docs/</strong><br data-start="1314" data-end="1317">
Documento técnico completo (PDF y archivos auxiliares).</p>
</li>
<li data-start="1377" data-end="1469">
<p data-start="1379" data-end="1469"><strong data-start="1379" data-end="1393">/hardware/</strong><br data-start="1393" data-end="1396">
Circuitos del puente trifásico, controlador de compuerta y bootstrap.</p>
</li>
<li data-start="1470" data-end="1612">
<p data-start="1472" data-end="1612"><strong data-start="1472" data-end="1490">/simulaciones/</strong><br data-start="1490" data-end="1493">
Códigos MATLAB utilizados para resolver las ecuaciones diferenciales y obtener resistencias de encendido y apagado.</p>
</li>
<li data-start="1613" data-end="1718">
<p data-start="1615" data-end="1718"><strong data-start="1615" data-end="1629">/firmware/</strong><br data-start="1629" data-end="1632">
Código para el ESP32 encargado de generar la secuencia de seis pasos mediante MCPWM.</p>
</li>
</ul>
<h2 data-start="1720" data-end="1749">Arquitectura del sistema</h2>
<ol data-start="1874" data-end="2170">
<li data-start="1874" data-end="1947">
<p data-start="1877" data-end="1947"><strong data-start="1877" data-end="1897">Fuente de poder:</strong> Alimentación de 12 V con filtrado en el bus DC.</p>
</li>
<li data-start="1948" data-end="2015">
<p data-start="1951" data-end="2015"><strong data-start="1951" data-end="1974">Bloque de potencia:</strong> Inversor trifásico con MOSFET IRFZ44N.</p>
</li>
<li data-start="2016" data-end="2170">
<p data-start="2019" data-end="2170"><strong data-start="2019" data-end="2041">Bloque de control:</strong> ESP32 configurado con MCPWM, convertidor de nivel y totem-pole para la compuerta del lado bajo, más bootstrap para el lado alto.</p>
</li>
</ol>
<h2 data-start="2172" data-end="2202">Resultados experimentales</h2>
<p data-start="2203" data-end="2323">El documento presenta mediciones reales de tiempos de conmutación (páginas 11–14):

| Modo            | Analítico (ns) | Experimental (ns) | Error (ns) |
|-----------------|----------------|-------------------|------------|
| Encendido bajo  | 172            | 112               | -60        |
| Apagado bajo    | 200            | 224               | 24         |
| Encendido alto  | 172            | 156               | -16        |
| Apagado alto    | 200            | 176               | -24        |

</div></div>
<p data-start="2758" data-end="2902">Las gráficas de osciloscopio incluidas en el documento muestran una correspondencia cuantitativa entre las formas de onda esperadas y observadas.</p>
<h2 data-start="2904" data-end="2927">Conclusiones</h2>
<ul data-start="3020" data-end="3351">
<li data-start="3020" data-end="3092">
<p data-start="3022" data-end="3092">El modelo analítico describe adecuadamente la dinámica de compuerta.</p>
</li>
<li data-start="3093" data-end="3163">
<p data-start="3095" data-end="3163">Las variaciones entre cálculo y experimento se mantienen acotadas.</p>
</li>
<li data-start="3164" data-end="3266">
<p data-start="3166" data-end="3266">El prototipo permite vincular de forma clara la formulación matemática con el comportamiento real.</p>
</li>
<li data-start="3267" data-end="3351">
<p data-start="3269" data-end="3351">El diseño es una base para versiones con mayor velocidad o menor pérdida dinámica.</p>
</li>
</ul>
<h2 data-start="3353" data-end="3398">Recomendaciones para futuros desarrollos</h2>
<ul data-start="3491" data-end="3789">
<li data-start="3491" data-end="3580">
<p data-start="3493" data-end="3580">Utilizar una <strong data-start="3506" data-end="3533">fuente lineal o batería</strong> para reducir oscilaciones en las mediciones.</p>
</li>
<li data-start="3581" data-end="3672">
<p data-start="3583" data-end="3672">Caracterizar en detalle el <strong data-start="3610" data-end="3634">convertidor de nivel</strong>, incluyendo tiempos de propagación.</p>
</li>
<li data-start="3673" data-end="3789">
<p data-start="3675" data-end="3789">Analizar con mayor precisión la <strong data-start="3707" data-end="3744">referencia flotante del lado alto</strong>, incorporándola explícitamente en el modelo.</p>
</li>
</ul>
<h2 data-start="3791" data-end="3813">Código de control</h2>
<p data-start="3814" data-end="3962">El firmware incluido implementa conmutación de seis pasos usando MCPWM del ESP32 (páginas 23–26) .<br data-start="3949" data-end="3952">
Incluye:</p>
<ul data-start="3963" data-end="4080">
<li data-start="3963" data-end="3990">
<p data-start="3965" data-end="3990">Inserción de dead-time.</p>
</li>
<li data-start="3991" data-end="4018">
<p data-start="3993" data-end="4018">Control del ciclo de trabajo.</p>
</li>
<li data-start="4019" data-end="4043">
<p data-start="4021" data-end="4043">Secuenciación S0–S5.</p>
</li>
</ul>
