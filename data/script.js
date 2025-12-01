const fileList = document.getElementById("fileList");
const refreshBtn = document.getElementById("refreshBtn");

async function fetchFiles() {
    try {
        const response = await fetch("/list-files");
        if (!response.ok) throw new Error("Failed to fetch file list");
        const files = await response.json();
        console.log(files);

        fileList.innerHTML = "";
        if (files.items  != undefined) {
            files.items.forEach(file => {
                const li = document.createElement("li");
                const a = document.createElement("a");
                a.textContent = file;
                a.href = "/file?fileName=" + file;
                li.appendChild(a);
                fileList.appendChild(li);
            });
        }

    } catch (err) {
        console.error(err);
        fileList.innerHTML = "<li>Error loading files</li>";
    }
}

refreshBtn.addEventListener("click", fetchFiles);
window.addEventListener("load", fetchFiles);