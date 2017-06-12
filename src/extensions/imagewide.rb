module ReVIEW
	class Compiler
		defblock :imagewide, 2..3, true
	end

	class Builder
		def imagewide(lines, id, caption, metric = nil)
			image(lines, id, caption, metric)
		end
	end

	class LATEXBuilder
		def imagewide(lines, id, caption, metric = nil)
			if @chapter.image(id).bound?
				imagewide_imagewide id, caption, metric
			else
				warn "image not bound: #{id}" if @strict
				image_dummy id, caption, lines
			end
		end

		def imagewide_imagewide(id, caption, metric)
			metrics = parse_metric("latex", metric)
			puts '\begin{reviewimage*}'
			if metrics.present?
				metrics.gsub!('\maxwidth', '\textwidth')
				puts "\\includegraphics[#{metrics}]{#{@chapter.image(id).path}}"
			else
				puts "\\includegraphics[width=\\textwidth]{#{@chapter.image(id).path}}"
			end

			if caption.present?
				puts macro('caption', compile_inline(caption))
			end

			puts macro('label', image_label(id))
			puts '\end{reviewimage*}'
		end
	end
end
