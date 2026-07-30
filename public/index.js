async function gerar(){

    const chave = document.getElementById("chave").value;

    const resposta = await fetch("/gerar",{
        method:"POST",
        headers:{
            "Content-Type":"application/json"
        },
        body:JSON.stringify({ chave })
    });

    const blob = await resposta.blob();

    const qr = document.getElementById("qr");
    const placeholder = document.getElementById("placeholder");

    qr.src = URL.createObjectURL(blob);
    qr.style.display = "block";
    placeholder.style.display = "none";
}
