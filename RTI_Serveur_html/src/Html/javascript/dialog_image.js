function afficherNomImage() {
    const input = document.getElementById('files');
    const nomImage = document.getElementById('label_selecteur');

    if (input.files.length > 0) {
        const nom = input.files[0].name;
        nomImage.textContent = nom;

        input.display = 'none';
    }
}
