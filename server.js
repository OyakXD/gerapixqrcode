const express = require('express');
const { spawn } = require("child_process");
const path = require("path");


function normalizarChave(chave){
	chave = chave.trim();

	if(chave.includes("@")){
		return chave.toLowerCase();
	}

	const numeros = chave.replace(/\D/g, "");

	return chave;
}

const app = express();

app.use(express.json());
app.use(express.static("public"));

app.post("/gerar", (req, res) => {
	
	const chave = normalizarChave(req.body.chave);

	const programa = spawn(path.join(__dirname, "src", "main"));

	res.setHeader("Content-Type", "image/png");
	
	const entrada = `chave=${chave}`

	programa.stdin.write(entrada);
	programa.stdin.end();

	programa.stdout.pipe(res);

	programa.stderr.on("data", (data) => {
		console.error(data.toString());
	});
});

app.listen(3000, () => {
	console.log("Servidor rodando em http://localhost:3000");
});
