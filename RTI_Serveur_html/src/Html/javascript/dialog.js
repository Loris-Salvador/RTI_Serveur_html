function ouvrirBoiteDialogue() {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "http://localhost:8080/api/1st_id", true);

    xhr.addEventListener("readystatechange", function() {

        if (xhr.readyState === 4) {
            if(xhr.status === 200)
            {
                var id = xhr.responseText;  // Récupérer le texte de la réponse
                document.getElementById('dialog_champ_id').textContent = id;

                document.getElementById('boiteDialogue').style.display = 'block';
            }
            else
                alert("Erreur: reception get /api/1st_id");
        }
    })
    xhr.send();

}

function fermerBoiteDialogue() {
    document.getElementById('boiteDialogue').style.display = 'none';
}

function Confirmer(){
    const cells = clickedRow.getElementsByTagName('td');

    var xhr = new XMLHttpRequest();
    xhr.open("POST", "http://localhost:8080/api", true);
    xhr.setRequestHeader("Content-Type", "application/json");

    var champId = document.getElementById('dialog_champ_id');
    var nouveauId = champId.textContent;

    const champIntitule = document.getElementById("dialog_champ_nom");
    let nouveauIntitule = champIntitule.value;


    const champPrix = document.getElementById("dialog_champ_prix");
    let nouveauPrix = champPrix.value;

    const regex = /^(\d+([.,]\d{1,2})?)?$/;
    if (!regex.test(nouveauPrix)) {
        alert("Erreur : Format de prix XX,XX ou XX.XX");
        return;
    }
    nouveauPrix = nouveauPrix.replace(/,/g, '.');


    const champQuantite = document.getElementById("dialog_champ_quantite");
    let nouvelleQuantite = champQuantite.value;

    const chiffresSeulement = /^\d+$/;

    if (!chiffresSeulement.test(nouvelleQuantite)) {
        alert("Erreur : Uniquement les chiffres sont accepte");
        return;
    }

    const champImage = document.getElementById('label_selecteur');
    let nouvelleImage = champImage.textContent;

    console.log("Id: " + nouveauId);
    console.log("Image: " + nouvelleImage);

    // Construire l'objet JSON avec les nouvelles données
    var data = {
        id : nouveauId,
        intitule : nouveauIntitule,
        quantite : nouvelleQuantite,
        prixUnitaire : nouveauPrix,
        image : nouvelleImage
    };

    xhr.addEventListener("readystatechange", function () {
        if (xhr.status === 201 && xhr.readyState === 4) {


            document.getElementById('boiteDialogue').style.display = 'none';

            clickedRow = null;
            refreshTable();
        }
    });

    // Convertir l'objet JSON en chaîne JSON et l'envoyer dans le corps de la requête
    xhr.send(JSON.stringify(data));
}



let deplacement = false;
let deplacementX = 0;
let deplacementY = 0;

function commencerDeplacement(e) {
    // Si l'élément cliqué est un champ de saisie, ne pas commencer le déplacement
    if (e.target.tagName.toLowerCase() === 'input' && e.target.type === 'text') {
        return;
    }

    e.preventDefault();
    deplacement = true;
    deplacementX = e.clientX - document.getElementById('boiteDialogue').offsetLeft;
    deplacementY = e.clientY - document.getElementById('boiteDialogue').offsetTop;
}

document.getElementById('boiteDialogue').addEventListener('mousedown', commencerDeplacement);


function deplacer(e) {
    if (deplacement) {
        let leftPosition = e.clientX - deplacementX;
        let topPosition = e.clientY - deplacementY;
        document.getElementById('boiteDialogue').style.left = leftPosition + 'px';
        document.getElementById('boiteDialogue').style.top = topPosition + 'px';
    }
}

function arreterDeplacement() {
    deplacement = false;
}

document.getElementById('boiteDialogue').addEventListener('mousedown', commencerDeplacement);
document.addEventListener('mousemove', deplacer);
document.addEventListener('mouseup', arreterDeplacement);

