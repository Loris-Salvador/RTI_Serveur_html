let clickedRow = null;

function refreshTable(){
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "http://localhost:8080/api", true);

    xhr.addEventListener("readystatechange", function() {

        if (xhr.status === 200 && xhr.readyState === 4) {
            var json = xhr.response;

            let articlesArray;
            try {
                articlesArray = JSON.parse(json);
            } catch (error) {
                console.error("Erreur lors de JSON.parse", error);
            }

            const tableBody = document.querySelector(".table_articles tbody");
            tableBody.innerHTML = "";

            for (let i = 0; i < articlesArray.length; i++) {
                let article = articlesArray[i];

                const row = document.createElement("tr");
                row.innerHTML = `
                  <td>${article.id}</td>
                  <td>${article.intitule}</td>
                  <td>${article.prixUnitaire}</td>
                  <td>${article.quantite}</td>`;


                row.addEventListener('click', () => {
                    const imageElement = document.getElementById("img_article");
                    imageElement.src = "image/image_article/" + article.image;

                    const rows = document.querySelectorAll('.table_articles tbody tr');
                    rows.forEach(row => {
                        row.classList.remove('clicked');
                    });

                    row.classList.add('clicked');
                    clickedRow = row;

                    const titreBox = document.getElementById("titre_box_article");

                    if(article.id<10)
                    {
                        if(article.intitule.length>10)
                            titreBox.innerHTML = `${article.id}: ${article.intitule.substring(0, 7)}...`;
                        else
                            titreBox.innerHTML = `${article.id}: ${article.intitule}`;

                    }
                    else
                    {
                        if(article.intitule.length>9)
                            titreBox.innerHTML = `${article.id}: ${article.intitule.substring(0, 6)}...`;
                        else
                            titreBox.innerHTML = `${article.id}: ${article.intitule}`;
                    }

                    console.log(article.intitule);
                    const champPrix = document.getElementById("champ_prix");
                    champPrix.value = `${article.prixUnitaire}`;

                    const champQuantite = document.getElementById("champ_quantite");
                    champQuantite.value = `${article.quantite}`;
                });
                tableBody.appendChild(row);

            }

            const firstRow = tableBody.querySelector('tr');

            const clickEvent = new Event('click');

            firstRow.dispatchEvent(clickEvent);

        }
    })

    xhr.send();
}

refreshTable();


function Modifier() {

    if (!clickedRow)
        return;


    const cells = clickedRow.getElementsByTagName('td');

    var xhr = new XMLHttpRequest();
    xhr.open("PUT", "http://localhost:8080/api?id=" + cells[0].innerText, true);
    xhr.setRequestHeader("Content-Type", "application/json");


    const champPrix = document.getElementById("champ_prix");
    let nouveauPrix = champPrix.value;

    const regex = /^(\d+([.,]\d{1,2})?)?$/;
    if (!regex.test(nouveauPrix)) {
        alert("Erreur : Format de prix XX,XX ou XX.XX");
        return;
    }
    nouveauPrix = nouveauPrix.replace(/,/g, '.');


    const champQuantite = document.getElementById("champ_quantite");
    let nouvelleQuantite = champQuantite.value;

    const chiffresSeulement = /^\d+$/;

    if (!chiffresSeulement.test(nouvelleQuantite)) {
        alert("Erreur : Uniquement les chiffres sont accepte");
        return;
    }

    // Construire l'objet JSON avec les nouvelles données
    var data = {
        prixUnitaire: nouveauPrix,
        quantite: nouvelleQuantite
    };

    xhr.addEventListener("readystatechange", function () {
        if (xhr.status === 200 && xhr.readyState === 4) {
            clickedRow = null;
            refreshTable();
        }
    });

    // Convertir l'objet JSON en chaîne JSON et l'envoyer dans le corps de la requête
    xhr.send(JSON.stringify(data));
}

function Supprimer(){

    if (!clickedRow)
        return;


    const cells = clickedRow.getElementsByTagName('td');

    var xhr = new XMLHttpRequest();
    xhr.open("DELETE", "http://localhost:8080/api?id=" + cells[0].innerText, true);
    xhr.setRequestHeader("Content-Type", "application/json");


    xhr.addEventListener("readystatechange", function () {
        if (xhr.status === 200 && xhr.readyState === 4) {
            clickedRow = null;
            refreshTable();
        }
    });

    // Convertir l'objet JSON en chaîne JSON et l'envoyer dans le corps de la requête
    xhr.send();
}
