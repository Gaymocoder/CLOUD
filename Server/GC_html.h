#include <iostream>

std::string Redirect(std::string ToPath)
{
	std::string HtmlCode = "\
	<html>\n\
		<head>\n\
			<meta http-equiv=\"refresh\" content=\"0; URL=" + ToPath + "\" />\n\
		</head>\n\
		<body>\n\
			Wait...\n\
		</body>\n\
	<html>";
	return HtmlCode;
}

std::string AccessError(std::string ToPath)
{
	std::string HtmlCode = "\
	<html>\n\
		<head>\n\
			<title>\n\
				Access denied!\n\
			</title>\n\
			<link rel=\"shortcut icon\" href=\"/sources/images/favicon.ico\" type=\"image/x-icon\">\n\
			<link rel=\"stylesheet\" type=\"text/css\" href=\"/sources/css/style.css\">\n\
		</head>\n\
		<body background=\"/sources/images/Cubes_Back.jpg\">\n\
			<div align=\"center\">\n\
				<div class=\"page-header\" align=\"center\">\n\
					Authorization failed\n\
				</div>\n\
			</div>\n\
			<div class=\"files\">\n\
				<img src=\"/sources/images/Access_Error.jpg\" width=\"100%\">\n\
				<div class=\"autherror\">\n\
					<div class=\"left-if\" align=\"center\" style=\"position: absolute\">\n\
						<div class=\"if\" align=\"center\">\n\
							But if you're truly the owner...\n\
						</div>\n\
					</div>\n\
					<div class=\"right-if\" align=\"center\" style=\"position: absolute\">\n\
						<div class=\"if\" align=\"center\">\n\
							...or the confident of the owner...\n\
						</div>\n\
					</div>\n\
					<img src=\"/sources/images/Logo.jpg\" style=\"position: relative; left: 35%\" width=\"30%\">\n\
				</div>\n\
				<div class=\"center-if\" align=\"center\">\n\
					...try again. With correct login and password.\n\
				</div>\n\
				<div class=\"log-in-error\" align=\"center\">\n\
					<form action=\"" + ToPath + "\" method=\"post\">\n\
						<table cellspacing=\"0\" align=\"center\" border=\"3\" width=\"300px\" height=\"300px\" bgcolor=\"white\">\n\
							<tr height=\"50\">\n\
								<td valing=\"middle\">\n\
									<h1 align=\"center\">LOG IN</h1>\n\
								</td>\n\
							</tr>\n\
							<tr border=\"0\">\n\
								<td valign=\"top\" align=\"center\">\n\
									<br>\n\
									Login<br>\n\
									<input name=\"Login\" type=\"text\" align=\"center\"><br>\n\
									<br>\n\
									Password<br>\n\
									<input name=\"Password\" type=\"password\" align=\"center\"><br>\n\
									<br><br>\n\
									<input value=\"Log in\" type=\"submit\">\n\
								</td>\n\
							</tr>\n\
						</table>\n\
					</form>\n\
				</div>\n\
			</div>\n\
		</body>\n\
	</html>\n";
	return HtmlCode;
}

std::string NonAuthIndex = "\
<html>\n\
	<head>\n\
		<title>\n\
			CLOUD\n\
		</title>\n\
		<link rel=\"shortcut icon\" href=\"/sources/images/favicon.ico\" type=\"image/x-icon\">\n\
		<link rel=\"stylesheet\" type=\"text/css\" href=\"/sources/css/style.css\">\n\
	</head>\n\
	<body background = \"/sources/images/Cubes_Back.jpg\">\n\
		<div class=\"title_logo\">\n\
			<img src=\"/sources/images/Logo.jpg\" alt=\"No image found\" align=\"right\" width=\"20%\">\n\
			<img src=\"/sources/images/Logo.png\" alt=\"No image found\" align=\"left\" width=\"77%\">\n\
		</div>\n\
		<form style=\"padding-top:25%\" action=\"wait.html\" method=\"post\">\n\
			<table cellspacing=\"0\" align=\"center\" border=\"3\" width=\"300px\" height=\"300px\" bgcolor=\"white\">\n\
				<tr height=\"50\">\n\
					<td valing=\"middle\">\n\
						<h1 align=\"center\">LOG IN</h1>\n\
					</td>\n\
				</tr>\n\
				<tr border=\"0\">\n\
					<td valign=\"top\" align=\"center\">\n\
						<br>\n\
						Login<br>\n\
						<input name=\"Login\" type=\"text\" align=\"center\"><br>\n\
						<br>\n\
						Password<br>\n\
						<input name=\"Password\" type=\"password\" align=\"center\"><br>\n\
						<br><br>\n\
						<input value=\"Log in\" type=\"submit\">\n\
					</td>\n\
				</tr>\n\
			</table>\n\
		</form>\n\
	</body>\n\
</html>\n";

std::string AuthIndex = "\
<html>\n\
	<head>\n\
		<title>\n\
			Your Cloud\n\
		</title>\n\
		<link rel=\"shortcut icon\" href=\"/sources/images/favicon.ico\" type=\"image/x-icon\">\n\
		<meta http-equiv=\"refresh\" content=\"0;URL=/yourcloud/\" />\n\
	</head>\n\
	<body>\n\
	</body>\n\
<html>\n";

std::string OverSizeError = "\
<html>\n\
	<head>\n\
		<title>\n\
			No place for file\n\
		</title>\n\
	</head>\n\
	<body>\n\
		<div class=\"size-error\" align=\"center\">\n\
			Uploading file resizes free place in your CLOUD\n\
		</div>\n\
	</body>\n\
</html>\n";
